#pragma once

#include "ui_DigitRecognition.h"
#include "Camera.h"
#include <QtWidgets/QMainWindow>
#include <QCameraInfo>

Q_DECLARE_METATYPE(cv::Mat);

class DigitRecognition : public QMainWindow
{
	Q_OBJECT

public:
	DigitRecognition(QWidget *parent = Q_NULLPTR);
	~DigitRecognition();

signals:
	void finishedRecognition();

private slots:
	void onStopStartButtonPressed() noexcept;
	void onAsyncImageCaptured(const cv::Mat& img);
	void onRecognizeButtonPressed();
	void updateWidgets();
	void clearWidgets();

private:
	void initializeDeviceBox() noexcept;
	void start();
	void stop();

	void reInitializeCNNParams();
	void changeGraydscaleMatLevel(cv::Mat& img);
	void drawCameraWindowRectangle(QPixmap& pixmap);
	void drawSubImages(const std::vector<cv::Mat>& images);
	void prepareResultString(const QVector<int>& result);
	void prepareCameraWindowsPixmap(cv::Mat& img);
	void prepareInvertedGraydscalePixmap(cv::Mat& img);

private:
	Ui::DigitRecognitionClass ui;
	Camera* m_camera = nullptr;
	QPixmap m_cameraWindowsPixmap;
	QPixmap m_subImagesPixmap;
	QPixmap m_invertedGrayscaledPixmap;
	QString m_resultString;
};
