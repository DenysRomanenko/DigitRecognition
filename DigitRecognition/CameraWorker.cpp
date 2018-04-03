#include "CameraWorker.h"
#include "Camera.h"

CameraWorker::CameraWorker(Camera* camera)
	: CameraWorker(UKNOWN_DEVICE, camera)
{}

CameraWorker::CameraWorker(int device, Camera* camera)
	: QObject(nullptr)
{
	m_camera = camera;
	setDevice(device);
}

CameraWorker::~CameraWorker()
{}

bool CameraWorker::setDevice(int device) noexcept
{
	if (m_isRunning) return false;
	m_device = device;
	return true;
}

bool CameraWorker::openDevice() noexcept {
	if (!m_videoCapture.open(m_device)) {
		m_isRunning = false;
		return false;
	}
}

void CameraWorker::start() {
	m_isRunning = true;
	cv::Mat image;
	while (m_isRunning) {
		QThread::msleep(40);
		m_videoCapture >> image;
		emit m_camera->asyncRenderedFrame(image);
	}
	m_videoCapture.release();
}

void CameraWorker::stop() {
	m_isRunning = false;
}