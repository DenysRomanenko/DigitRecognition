#include "CNNEngine.h"
#include "DigitRecognition.h"
#include "Constants.h"

#include <algorithm>
#include <opencv2\imgproc\imgproc.hpp>
#include <QPalette>
#include <QPainter>

DigitRecognition::DigitRecognition(QWidget *parent)
	: QMainWindow(parent)
{
	qRegisterMetaType<cv::Mat>();
	ui.setupUi(this);
	setFixedSize(this->size());

	m_camera = new Camera(this);
	initializeDeviceBox();
	
	connect(ui.stopStartButton, SIGNAL(clicked()), this, SLOT(onStopStartButtonPressed()));
	connect(this, SIGNAL(finishedRecognition()), this, SLOT(updateWidgets()), Qt::QueuedConnection);
	connect(ui.clearButton, SIGNAL(clicked()), ui.drawWindow, SLOT(clearWindow()));
	connect(ui.clearButton, SIGNAL(clicked()), this, SLOT(clearWidgets()));
	connect(ui.recognizeButton, SIGNAL(clicked()), this, SLOT(onRecognizeButtonPressed()));
}

DigitRecognition::~DigitRecognition() {
	stop();
}

void DigitRecognition::initializeDeviceBox() noexcept {
	const auto m_deviceInfoList = QCameraInfo::availableCameras();
	auto box = ui.deviceBox;

	box->clear();
	for (const auto& info : m_deviceInfoList) {
		box->addItem(info.description());
	}
}

void DigitRecognition::onStopStartButtonPressed() noexcept{
	auto button = ui.stopStartButton;
	if (button->text() == "Start"){
		button->setText("Stop");
		ui.deviceBox->setEnabled(false);
		start();
	}
	else {
		stop();
		button->setText("Start");
		ui.deviceBox->setEnabled(true);
		QTimer::singleShot(100, this, SLOT(clearWidgets()));
	}
}

void DigitRecognition::start() {
	connect(m_camera, SIGNAL(asyncRenderedFrame(const cv::Mat&)), this, SLOT(onAsyncImageCaptured(const cv::Mat&)), Qt::DirectConnection);
	m_camera->setDevice(ui.deviceBox->currentIndex());
	m_camera->start();
}

void DigitRecognition::stop() {
	if (!m_camera->isRunning()) return;
	disconnect(m_camera, SIGNAL(asyncRenderedFrame(const cv::Mat&)));
	m_camera->stop();
	ui.cameraWindow->clear();
	ui.subImagesWindow->clear();
	ui.invertedGrayscaledWindow->clear();
}

void DigitRecognition::onAsyncImageCaptured(const cv::Mat& img) {
	//Convert RGB2GRAY
	cv::Mat grayscaled = cv::Mat(img, cv::Rect(X_POSITION, Y_POSITION, ANALYZING_IMAGE_WIDTH, ANALYZING_IMAGE_HEIGHT));;
	cv::cvtColor(grayscaled, grayscaled, cv::COLOR_BGR2GRAY);
	//Convert white colors to black
	cv::Mat invertedGrayScaled;
	cv::bitwise_not(grayscaled, invertedGrayScaled);
	changeGraydscaleMatLevel(invertedGrayScaled);
	
	//Recognize numbers	
	reInitializeCNNParams();
	auto& ai = CNNEngine::instance();
	std::vector<cv::Mat> subImages;
	const auto res = ai.recognizeNumbers(invertedGrayScaled, subImages);
	
	//Prepare data for output
	prepareResultString(res);
	
	cv::Mat rgbImage;
	cv::cvtColor(img, rgbImage, CV_BGR2RGB);
	prepareCameraWindowsPixmap(rgbImage);
	
	prepareInvertedGraydscalePixmap(invertedGrayScaled);
	drawSubImages(subImages);
	
	emit finishedRecognition();
}

void DigitRecognition::prepareInvertedGraydscalePixmap(cv::Mat& img) {
	const auto windowSize = ui.invertedGrayscaledWindow->size();
	cv::resize(img, img, cv::Size(windowSize.width(), windowSize.height()));
	const auto invertedGrayscaledImage = QImage((uchar*)img.data, img.cols, img.rows,
														img.step, QImage::Format_Grayscale8);
	m_invertedGrayscaledPixmap = QPixmap::fromImage(invertedGrayscaledImage);
}

void DigitRecognition::prepareCameraWindowsPixmap(cv::Mat& img) {
	const auto cameraWindowsImage = QImage( (uchar*)img.data, img.cols, img.rows,
													img.step, QImage::Format_RGB888);
	m_cameraWindowsPixmap = QPixmap::fromImage(cameraWindowsImage);
	drawCameraWindowRectangle(m_cameraWindowsPixmap);
}


void DigitRecognition::prepareResultString(const QVector<int>& result) {
	QString printStr;
	printStr.reserve(100);

	printStr += "Result: ";
	for (auto it : result) {
		printStr += QString::number(it);
		printStr += ' ';
	}

	m_resultString = printStr;
}

void DigitRecognition::reInitializeCNNParams() {
	auto& ai = CNNEngine::instance();
	ai.setRate(ui.rateSpin->value());
	ai.setStep(ui.stepSpin->value());
	ai.setMinSum(ui.minSpin->value());
	ai.setMaxSum(ui.maxSpin->value());
	const int val = ui.digitSizeSpin->value();
	ai.setDigitSize(val, val);
	ai.setAi(ui.aiCheckBox->isChecked());
}

void DigitRecognition::updateWidgets() {
	ui.cameraWindow->setPixmap(m_cameraWindowsPixmap);
	ui.subImagesWindow->setPixmap(m_subImagesPixmap);
	ui.invertedGrayscaledWindow->setPixmap(m_invertedGrayscaledPixmap);
	ui.resultWindow->setText(m_resultString);
}

void DigitRecognition::drawCameraWindowRectangle(QPixmap& pixmap) {
	if (pixmap.size() != QSize(CAPTURED_IMAGE_WIDTH, CAPTURED_IMAGE_HEIGHT))
		return;

	QPainter p(&pixmap);
	p.setPen(QPen(Qt::red, 2));
	const QVector<QLine> lines = {  QLine{ X_POSITION, Y_POSITION, X_POSITION + ANALYZING_IMAGE_WIDTH, Y_POSITION  },
									QLine{ X_POSITION, Y_POSITION, X_POSITION, Y_POSITION + ANALYZING_IMAGE_HEIGHT },
									QLine{ X_POSITION, Y_POSITION + ANALYZING_IMAGE_HEIGHT, X_POSITION + ANALYZING_IMAGE_WIDTH, Y_POSITION + ANALYZING_IMAGE_HEIGHT },
									QLine{ X_POSITION + ANALYZING_IMAGE_WIDTH, Y_POSITION, X_POSITION + ANALYZING_IMAGE_WIDTH, Y_POSITION + ANALYZING_IMAGE_HEIGHT } };
	p.drawLines(lines);
	p.end();
}

void DigitRecognition::changeGraydscaleMatLevel(cv::Mat& img) {
	for (auto start = img.data; start < img.dataend; ++start) {
		if (*start <= ui.conditionSpin->value())
			*start = std::max(0, *start - ui.blackSpin->value());
		else
			*start = std::min(255, *start + ui.whiteSpin->value());
	}
}

void DigitRecognition::drawSubImages(const std::vector<cv::Mat>& images) {
	m_subImagesPixmap = QPixmap(ui.subImagesWindow->size());
	const auto totalImagesCount = images.size();
	if (images.empty()) return;
	const auto windowSize = ui.subImagesWindow->size();
	
	QPainter p;
	p.begin(&m_subImagesPixmap);
	
	int drawedImageNumber = 0;
	for (int i = 0; i < windowSize.height(); i += INPUT_HEIGHT) {
		for (int j = 0; j < windowSize.width(); j += INPUT_WIDTH) {
			const auto& tmp = images[drawedImageNumber];
			p.drawPixmap(i, j, QPixmap::fromImage(QImage((uchar*)tmp.data, tmp.cols, tmp.rows,
					tmp.step, QImage::Format_Grayscale8)));
			
			++drawedImageNumber;
			if (drawedImageNumber >= totalImagesCount)
				goto CYCLE_EXIT;
		}
	}
CYCLE_EXIT:
	p.end();
}

void DigitRecognition::onRecognizeButtonPressed() {
	auto& ai = CNNEngine::instance();
	auto res = ai.recognizeNumbers(ui.drawWindow->getImages());
	prepareResultString(res);
	updateWidgets();
}

void DigitRecognition::clearWidgets() {
	ui.cameraWindow->clear(); 
	ui.subImagesWindow->clear(); 
	ui.invertedGrayscaledWindow->clear(); 
	ui.resultWindow->clear();
}