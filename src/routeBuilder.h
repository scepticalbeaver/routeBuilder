#pragma once

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtCore/QFile>
#include <QtCore/QStringList>

#include <QtCore/QDebug>

#include "trikControl/brick.h"

using namespace trikControl;

class RouteBuilder : public QObject
{
	Q_OBJECT
public:
	explicit RouteBuilder(QThread *mainThread);

	QStringList motorList();
	QStringList digitalList();
	float readSomeSensor();


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

	unsigned int const trackingTimeout = 1000; // msec

	QFile *mCollectedData;
	Brick mBrick;
	QTimer mTracker;
	State mWorkMode;
	QStringList mTrackingMotors;
	unsigned int mTrackingCounter;

	void init();
	void initTracker();


protected slots:
	void readSensors();
};
