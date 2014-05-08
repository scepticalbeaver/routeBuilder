#include <QtGui/QApplication>
#include <QObject>

#include "motorDevice.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	QTimer timer;
	tests::MotorDevice motor;

	motor.setPower(70);
	QObject::connect(&timer, SIGNAL(timeout()), &motor, SLOT(readEncoder()));

	timer.start(1000);

	return app.exec();
}

