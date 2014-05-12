#pragma once

#include <QtCore/QDebug>

#include "trackStorage.h"
#include "routeRepeater.h"
#include "deviceExplorer.h"

//! @class RouteController is a facade of route-repeat tech
//! may be used in other thread
class RouteController : public QObject
{
	Q_OBJECT
public:
	explicit RouteController(QThread *guiThread);
	~RouteController();

	//! creates class objects after moving in new thread
	//! fixes miss of moveToThread of child objects
	void afterThreadInit();

public slots:
	//! will recognize devices with their ports
	void initDevices();
	void checkLoadedDevices();
	void startTracking();
	void stopTracking();
	void loadTrackFromFile();
	void playback();
	void switchMotors(bool const willTurnOn);

signals:
	void jobDone(bool);

protected:
	QThread *mGuiThread;
	DeviceExplorer *mDeviceInfo;
	TrackStorage *mStorage;
	RouteRepeater *mRouteRepeater;
	QVector<MotorComplect *> *mMotorsComplect;

	void turnPowerMotors(int const power);
	void checkRAII();
	void resetEncoders();

protected slots:
	void playbackStopped();
};
