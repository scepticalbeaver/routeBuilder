#include "routeController.h"

using namespace trikControl;

RouteController::RouteController(QThread *guiThread)
	: mCollectedData(nullptr)
	, mBrick(*guiThread, "./")
	, mWorkMode(sleep)
	, mTrackingCounter(0)
{
	qDebug() << "--constructing RouteBuilder...";
	qDebug() << "voltage: \t" << mBrick.battery()->readVoltage();

}

QStringList RouteController::motorList()
{
	foreach (QString motorPort, mBrick.motorPorts(Motor::powerMotor))
	{
		Motor *someEnc = mBrick.motor(motorPort);
		if (someEnc != nullptr)
		{
			qDebug() << "--Works: " << motorPort;
		}
		else
		{
			qDebug() << "-- null pointer on: " << motorPort;
		}
	}
	return mBrick.motorPorts(Motor::powerMotor);
}

float RouteController::readSomeSensor()
{
	QStringList encoders;
	encoders << "JB1" << "JB2" << "JB3" << "JB4";

	float result = -1;

	foreach (QString encoderPort, encoders)
	{
		Encoder *someEnc = mBrick.encoder(encoderPort);
		if (someEnc == nullptr)
		{
			qDebug() << "--null pointer at\t" << encoderPort;
			continue;
		}

		qDebug() << encoderPort << " -> \t" << someEnc->read();

		result = someEnc->read();
	}


	return result;
}

void RouteController::switchPowerMotors(int const power)
{
	qDebug() << "-- try set power on each motor";

	foreach (QString const motorPort, mBrick.motorPorts(Motor::powerMotor))
	{
		Motor *someMotor = mBrick.motor(motorPort);
		if (someMotor == nullptr)
		{
			qDebug() << "--motor is NULL";
			return;
		}
		qDebug() << "--got pointer";
		someMotor->setPower(power);
		qDebug() << "cur power on\t" << motorPort << "is \t" << someMotor->power();
	}

}


void RouteController::startTracking()
{
	mWorkMode = trackRoute;
	init();
}

void RouteController::stopTracking()
{
	qDebug() << "--stop signal...";
	mTracker.stop();

	if (mCollectedData->isOpen())
	{
		mCollectedData->close();
	}


	switch (mWorkMode)
	{
	case sleep: break;
	case trackRoute:
		disconnect(this, SLOT(readSensors()));
		mStorage.printToFile();
		break;
	case repeatRoute:
		break;
	}
	qDebug() << "--end of tracking task";

	switchPowerMotors(0);
}

void RouteController::playback()
{
	mRouteRepeater.playback();
}

void RouteController::switchMotors(const bool willTurnOn)
{
	int const power = (willTurnOn)? 85 : 0;
	switchPowerMotors(power);
}


void RouteController::init()
{

}

void RouteController::initTracker()
{
	qDebug() << "--Tracker initialization...";
	mCollectedData = new QFile("routeBuilder_collected" + QString::number(mBrick.time() % 10000) + ".txt");
	if (!mCollectedData->open(QFile::WriteOnly))
	{
		qDebug() << "--Some problem with file opening!";
		return;
	}

	qDebug() << "--getting motor ports...";
	mTrackingMotors << "JB2";

	resetEncoders();

	if (!mTrackingMotors.count())
	{
		qDebug() << "no ports";
		return;
	}

	connect(&mTracker, SIGNAL(timeout()), SLOT(readSensors()));
	mTracker.start(trackingTimeout);
}

void RouteController::resetEncoders()
{
	QStringList encoders;
	encoders << "JB1" << "JB2" << "JB3" << "JB4";

	float result = -1;

	foreach (QString encoderPort, encoders)
	{
		Encoder *someEnc = mBrick.encoder(encoderPort);
		if (someEnc == nullptr)
		{
			continue;
		}
		someEnc->reset();
	}
}

void RouteController::readSensors()
{
	QString line = "";

	float currValue = mBrick.encoder("JB2")->read();

	line = "JB2\t" + QString::number(currValue) + "\n";
	mCollectedData->write(line.toUtf8());

	mStorage.addValue(currValue);

	mTrackingCounter++;
	if (mTrackingCounter % (1000 / trackingTimeout) == 0)
	{
		mCollectedData->flush();
		qDebug() << currValue;
	}
}
