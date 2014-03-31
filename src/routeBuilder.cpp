#include "routeBuilder.h"

RouteBuilder::RouteBuilder(QThread *mainThread)
	: mCollectedData(nullptr)
	, mBrick(*mainThread, "./")
	, mWorkMode(sleep)
	, mTrackingCounter(0)
{
	qDebug() << "--constructing RouteBuilder...";
	qDebug() << "voltage: \t" << mBrick.battery()->readVoltage();

}

QStringList RouteBuilder::motorList()
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

float RouteBuilder::readSomeSensor()
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

void RouteBuilder::setPowerOnMotor(int const power)
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


void RouteBuilder::startTracking()
{
	mWorkMode = trackRoute;
	init();
}

void RouteBuilder::stop()
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

	setPowerOnMotor(0);
}

void RouteBuilder::init()
{
	qDebug() << "--initialization...";
	switch (mWorkMode)
	{
	case sleep: break;
	case trackRoute:
		initTracker();
		break;
	case repeatRoute: break;
	}
}

void RouteBuilder::initTracker()
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

void RouteBuilder::resetEncoders()
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

void RouteBuilder::readSensors()
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
