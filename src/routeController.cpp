#include "routeController.h"

using namespace trikControl;

RouteController::RouteController(QThread *guiThread)
	: mBrick(*guiThread, "./")
{
	mMotorsComplect = new QVector<MotorComplect>;
	mStorage = new TrackStorage(mMotorsComplect, this);
	mRouteRepeater = new RouteRepeater(mStorage, this);
}

RouteController::~RouteController()
{
	delete mMotorsComplect;
}

QList<Motor *> RouteController::motorList()
{
	QList<Motor *> result;

	foreach(QString &port, mBrick.motorPorts(Motor::powerMotor))
	{
		Motor *motor = mBrick.motor(port);
		if (motor != nullptr)
		{
			result << motor;
		}
	}

	return result;
}

QList<Encoder *> RouteController::encoderList()
{
	QList<Encoder *> result;
	QStringList encoders;
	encoders << "JB1" << "JB2" << "JB3" << "JB4";

	foreach (QString &ePort, encoders)
	{
		Encoder *encoder = mBrick.encoder(ePort);
		if (encoder != nullptr)
		{
			result << encoder;
		}
	}
	return result;
}

void RouteController::sleep(unsigned int const &msec)
{
	QEventLoop loop;
	QTimer::singleShot(msec, &loop, SLOT(quit()));
	loop.exec();
}

void RouteController::switchPowerMotors(int const power)
{
	foreach(Motor *motor, motorList())
	{
		motor->setPower(power);
	}
}


void RouteController::startTracking()
{
	if (!mStorage.startRecording())
	{
		qDebug() << "init devices first!";
	}
}

void RouteController::stopTracking()
{
	mStorage->stopRecording();
}

void RouteController::playback()
{
	mRouteRepeater.playback();
}

void RouteController::switchMotors(bool const willTurnOn)
{
	int const power = (willTurnOn)? 85 : 0;
	switchPowerMotors(power);
}

void RouteController::initDevices()
{
	int const testPower = 50;
	foreach (Motor *motor, motorList())
	{
		resetEncoders();
		motor->setPower(testPower);
		sleep(1000);
		motor->setPower(0);

		float max = 0;
		Encoder *muchedEncoder = nullptr;
		foreach(Encoder *encoder, encoderList())
		{
			if (qAbs(encoder->read()) > max)
			{
				max = qAbs(encoder->read());
				muchedEncoder = encoder;
			}
		}
		mMotorsComplect << MotorComplect(motor, muchedEncoder);
	}
}

void RouteController::resetEncoders()
{
	foreach (Encoder *encoder, encoderList())
	{
		encoder->reset();
	}
}
