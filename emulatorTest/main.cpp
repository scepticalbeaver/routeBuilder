#include <QCoreApplication>
#include <QObject>

#include "motorDevice.h"

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);

	QTimer timer;
	emulators::MotorDevice motor;

	motor.setPower(70);
	QObject::connect(&timer, SIGNAL(timeout()), &motor, SLOT(readEncoder()));

	timer.start(500);

	return app.exec();
}

