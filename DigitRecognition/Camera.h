#pragma once
#include <memory>
#include <QtCore>
#include <QWidget>
#include <QCameraInfo>
#include <opencv2\opencv.hpp>

#include "CameraWorker.h"

constexpr int UKNOWN_DEVICE = -1;

class Camera : public QObject
{
	Q_OBJECT
public:
	Camera(QObject *parent = nullptr);
	Camera(int device, QObject *parent = nullptr);
	Camera(const Camera&) = delete;
	Camera(Camera&&) = delete;
	~Camera();

	bool setDevice(int device) noexcept { return m_cameraWorker->setDevice(device); };
	static QList<QCameraInfo> getAvailableDevices() noexcept;

	bool start();
	void stop();
	bool isRunning() const noexcept { return m_cameraWorker->isRunning(); }

signals:
	void asyncRenderedFrame(const cv::Mat& image);

private:
	void initializeThread();

private:
	QMutex							m_mutex;
	QThread*						m_workingThread = nullptr;
	std::unique_ptr<CameraWorker>	m_cameraWorker	= nullptr;
};
