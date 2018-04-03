#include "Camera.h"
#include <QCameraInfo>
#include <QPainter>
#include <QApplication>

Camera::Camera(QObject *parent)
	: Camera(UKNOWN_DEVICE, parent)
{}

Camera::Camera(int device, QObject *parent)
	: QObject(parent)
{
	m_cameraWorker = std::make_unique<CameraWorker>(this);
	setDevice(device);
}

Camera::~Camera()
{
	stop();
}

QList<QCameraInfo> Camera::getAvailableDevices() noexcept {
	return QCameraInfo::availableCameras();
}

bool Camera::start() {
	if (isRunning()) return true;
	QMutexLocker locker(&m_mutex);
	if (!m_cameraWorker->openDevice()) return false;
	initializeThread();
	m_workingThread->start();
	return true;
}

void Camera::stop() {
	if (!isRunning()) return;
	QMutexLocker locker(&m_mutex);
	m_cameraWorker->stop();
	m_workingThread->quit();
	m_workingThread->wait();
	m_workingThread->deleteLater();
}

void Camera::initializeThread() {
	m_workingThread = new QThread(this);
	connect(m_workingThread, SIGNAL(started()), m_cameraWorker.get(), SLOT(start()), Qt::DirectConnection);
	m_cameraWorker->moveToThread(m_workingThread);
}