#pragma once

#include <QObject>
#include <opencv2\opencv.hpp>

class Camera;

class CameraWorker : public QObject
{
	Q_OBJECT

public:
	CameraWorker(Camera* parent);
	CameraWorker(int device, Camera* parent);
	CameraWorker(const CameraWorker&) = delete;
	CameraWorker(CameraWorker&&) = delete;
	~CameraWorker();

	bool setDevice(int device) noexcept;
	bool openDevice() noexcept;
	bool isRunning() const noexcept { return m_isRunning; }

public slots:
	void start();
	void stop();

private:
	cv::VideoCapture	m_videoCapture;
	Camera*				m_camera	= nullptr;
	int					m_device;
	bool				m_isRunning = false;
};
