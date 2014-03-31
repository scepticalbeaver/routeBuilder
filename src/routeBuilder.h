#pragma once

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtCore/QFile>
#include <QtCore/QStringList>

#include <QtCore/QDebug>

#include "trikControl/brick.h"
#include "trackStorage.h"

using namespace trikControl;

class RouteBuilder : public QObject
{
	Q_OBJECT
public:
	explicit RouteBuilder(QThread *mainThread);

	QStringList motorList();
	float readSomeSensor();

	void setPowerOnMotor(int const power);


public slots:
	void startTracking();
	void stop();

protected:
	enum State
	{
		sleep
		, trackRoute
		, repeatRoute
	};

	unsigned int const trackingTimeout = 100; // msec

	QFile *mCollectedData;
	Brick mBrick;
	QTimer mTracker;
	TrackStorage mStorage;
	State mWorkMode;
	QStringList mTrackingMotors;
	unsigned int mTrackingCounter;

	void init();
	void initTracker();
	void resetEncoders();


protected slots:
	void readSensors();
};
