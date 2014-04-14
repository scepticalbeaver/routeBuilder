#include "routeController.h"

using namespace trikControl;

RouteController::RouteController(QThread *guiThread)
	: mBrick(*guiThread, "./")
{
	mMotorsComplect = new QVector<MotorComplect *>;
	mStorage = new TrackStorage(mMotorsComplect, this);
	mRouteRepeater = new RouteRepeater(mStorage, this);

	qDebug() << "-- gui thread " << guiThread;
}

RouteController::~RouteController()
{
//	delete mRouteRepeater;
//	delete mStorage;

	foreach(MotorComplect *complect, *mMotorsComplect)
	{
		delete complect;
	}

	delete mMotorsComplect;
}

QList<Motor *> RouteController::motorList()
{
	QList<Motor *> result;

	foreach(QString const &port, mBrick.motorPorts(Motor::powerMotor))
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
	encoders << "JB1" << "JB2" << "JB3" << "JB4" << "JM1" << "JM2" << "JM3" << "M1";

	foreach (QString const &ePort, encoders)
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
	resetEncoders();
	mStorage->startRecording();
	emit jobDone(true);
}

void RouteController::stopTracking()
{
	mStorage->stopRecording();
	emit jobDone(true);
}

void RouteController::playback()
{
	qDebug() << "controller, try reset encoder";
	resetEncoders();
	mRouteRepeater->playback();
	emit jobDone(true);
}

void RouteController::switchMotors(bool const willTurnOn)
{
	int const power = (willTurnOn)? 85 : 0;
	switchPowerMotors(power);
	emit jobDone(true);
}

void RouteController::initDevices()
{
	int const testPower = 60;
	float const epsilon = 30;
	foreach (Motor *motor, motorList())
	{
		resetEncoders();
		motor->setPower(testPower);
		sleep(500);
		motor->setPower(0);

		float max = 0;
		Encoder *machedEncoder = nullptr;
		foreach(Encoder *encoder, encoderList())
		{
			if (qAbs(encoder->read()) < epsilon)
			{
				continue;
			}
			if (qAbs(encoder->read()) > max)
			{
				max = qAbs(encoder->read());
				machedEncoder = encoder;
			}
		}
		if (machedEncoder != nullptr)
		{
			qDebug() << "(motor, encoder) = " << motor << " " << machedEncoder;
			mMotorsComplect->append(new MotorComplect(motor, machedEncoder, mMotorsComplect->size()));
			qDebug() << "reversed?" << machedEncoder->read();
			mMotorsComplect->last()->setReversed(machedEncoder->read() < 0);
		}
	}
	qDebug() << "motors founded: " << mMotorsComplect->size();
	emit jobDone(true);
}

void RouteController::resetEncoders()
{
	foreach (Encoder *encoder, encoderList())
	{
		encoder->reset();
	}
}
