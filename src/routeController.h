#pragma once

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtCore/QFile>
#include <QtCore/QStringList>

#include <QtCore/QDebug>

#include "trikControl/brick.h"
#include "trackStorage.h"
#include "routeRepeater.h"

class RouteController : public QObject
{
	Q_OBJECT
public:
	explicit RouteController(QThread *guiThread);

	QStringList motorList();
	float readSomeSensor();


public slots:
	void startTracking();
	void stopTracking();
	void playback();
	void switchMotors(bool const willTurnOn);


protected:
	unsigned int const trackingTimeout = 100; // msec

	QFile *mCollectedData;
	trikControl::Brick mBrick;
	QTimer mTracker;
	TrackStorage mStorage;
	RouteRepeater mRouteRepeater;
	State mWorkMode;
	QStringList mTrackingMotors;
	unsigned int mTrackingCounter;

	//! will recognize devices with their ports
	void initDevices();
	void initTracker();
	void resetEncoders();
	void switchPowerMotors(int const power);


protected slots:
	void readSensors();
};
