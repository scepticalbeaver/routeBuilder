#include "routeBuilder.h"

RouteBuilder::RouteBuilder(QThread *mainThread)
	: mCollectedData(nullptr)
	, mBrick(*mainThread, "./")
	, mWorkMode(sleep)
	, mTrackingCounter(0)
{
}

QStringList RouteBuilder::motorList()
{
	return mBrick.motorPorts(Motor::powerMotor);
}

QStringList RouteBuilder::digitalList()
{
	return mBrick.sensorPorts(Sensor::digitalSensor);
}

float RouteBuilder::readSomeSensor()
{
	return mBrick.encoder(motorList().first())->read();
}

void RouteBuilder::startTracking()
{
	mWorkMode = trackRoute;
	init();
}

void RouteBuilder::stop()
{
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
		break;
	case repeatRoute:
		break;
	}
}

void RouteBuilder::init()
{
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
	mCollectedData = new QFile("routeBuilder_collected" + QString::number(mBrick.time() % 10000) + ".txt");
	if (!mCollectedData->open(QFile::WriteOnly))
	{
		qDebug() << "--Some problem with file opening!";
		return;
	}

	mTrackingMotors << mBrick.motorPorts(Motor::powerMotor);

	//mTrackingMotors << mBrick.sensorPorts(Sensor::digitalSensor);

	if (!mTrackingMotors.count())
	{
		qDebug() << "no ports";
		return;
	}


	qDebug() << "--port counts: " << mTrackingMotors.count();
	qDebug() << "--first\t" << mTrackingMotors.first();
	qDebug() << "--last\t" << mTrackingMotors.last();



	connect(&mTracker, SIGNAL(timeout()), SLOT(readSensors()));
	mTracker.start(trackingTimeout);
}

void RouteBuilder::readSensors()
{
	QString line = "";
	foreach (QString const motor, mTrackingMotors)
	{
		//line = motor + " : \t\t" + QString::number(mBrick.sensor(motor)->read()) + "\n";
		line = motor + " : \t\t" + QString::number(mBrick.encoder(motor)->read()) + "\n";
		qDebug() <<  line;
		//mCollectedData->write(line.toUtf8());
	}


	/*mTrackingCounter++;
	if (mTrackingCounter % (1000 / trackingTimeout) == 0)
	{
		mCollectedData->flush();
	}*/
}
