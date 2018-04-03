#include "DrawWindow.h"

#include <QPen>
#include <QMouseEvent>
#include <opencv2\imgproc\imgproc.hpp>

constexpr auto defaultTexturePath = "files\\texture.png";
constexpr int totalImages = 20;
constexpr int rowImageCount = 4;
constexpr int colImageCount = 5;
constexpr int imageWidth = 112;
constexpr int imageHeight = 112;
constexpr int step = 1;

DrawWindow::DrawWindow(QWidget *parent)
	: QLabel(parent),
	m_defaulTexture(defaultTexturePath),
	m_pen{ Qt::white, 5, Qt::SolidLine, Qt::RoundCap }
{
	ui.setupUi(this);
	clearWindow();
}

DrawWindow::~DrawWindow()
{
}

void DrawWindow::mouseMoveEvent(QMouseEvent *ev) {
	if (!(ev->buttons() & Qt::MouseButton::LeftButton)) {
		m_prevPoint = QPoint();
		return;
	}
	m_currentPoint = ev->pos();
	repaint();
	m_prevPoint = m_currentPoint;
	QLabel::mouseMoveEvent(ev);
}

void DrawWindow::paintEvent(QPaintEvent *ev) {
	QPixmap pixmap(*(this->pixmap()));
	QPainter painter(&pixmap);
	painter.setPen(m_pen);
	
	if (!m_prevPoint.isNull()) {
		painter.drawLine(m_prevPoint, m_currentPoint);
	}
	
	painter.end();
	setPixmap(pixmap);
	QLabel::paintEvent(ev);
}

void DrawWindow::mousePressEvent(QMouseEvent *ev) {
	m_prevPoint = QPoint();
	QLabel::mousePressEvent(ev);
};

void DrawWindow::mouseReleaseEvent(QMouseEvent *ev) {
	m_prevPoint = QPoint();
	QLabel::mouseReleaseEvent(ev);
}

void DrawWindow::clearWindow() {
	setPixmap(QPixmap::fromImage(m_defaulTexture));
}

std::vector<cv::Mat> DrawWindow::getImages() const noexcept {
	std::vector<cv::Mat> images(totalImages);
	
	auto windowImage = this->pixmap()->toImage();
	for (int i = 0, x = 0, y = 0; i < totalImages; ++i, x += (imageWidth + step)) {
		if ((i % colImageCount == 0) && (i != 0)) {
			x = 0;
			y += (imageHeight + step);
		}
		auto batch = windowImage.copy(QRect(x, y, imageWidth, imageHeight));
		batch = batch.convertToFormat(QImage::Format_RGB888);
		auto mat = cv::Mat(cv::Size(batch.width(), batch.height()), CV_8UC3, batch.bits(), batch.bytesPerLine());
		cv::cvtColor(mat, images[i], cv::COLOR_BGR2GRAY);
	}

	return images;
}