#define NO_STRICT
#define NOMINMAX
//#define WIN32_LEAN_AND_MEAN
//#define GLOG_NO_ABBREVIATED_SEVERITIES

#include "CNNEngine.h"
#include "DigitRecognition.h"
#include <QtWidgets/QApplication>
#include <QMessageBox>

using namespace cv;
int main(int argc, char *argv[])
{
	::google::InitGoogleLogging(argv[0]);
	QApplication a(argc, argv);

	auto& instance = CNNEngine::instance();
	instance.initialize();

	if (!instance.isInitialized()) {
		QMessageBox::critical(nullptr, "Error", "CNN engine is not initialized()", QMessageBox::StandardButton::Ok);
		return -1;
	}

	DigitRecognition w;
	w.show();
	return a.exec();
}
