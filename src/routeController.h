#pragma once

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtCore/QFile>
#include <QtCore/QEventLoop>
#include <QtCore/QStringList>

#include <QtCore/QDebug>

#include "trikControl/brick.h"
#include "trackStorage.h"
#include "routeRepeater.h"
#include "motorComplect.h"

class RouteController : public QObject
{
	Q_OBJECT
public:
	explicit RouteController(QThread *guiThread);
	~RouteController();


public slots:
	//! will recognize devices with their ports
	void initDevices();
	void startTracking();
	void stopTracking();
	void playback();
	void switchMotors(bool const willTurnOn);

signals:
	void jobDone(bool);

protected:
	unsigned int const trackingTimeout = 100; // msec

	trikControl::Brick mBrick;
	TrackStorage *mStorage;
	RouteRepeater *mRouteRepeater;
	QVector<MotorComplect *> *mMotorsComplect;

	void resetEncoders();
	void switchPowerMotors(int const power);
	QList<trikControl::Motor *> motorList();
	QList<trikControl::Encoder *> encoderList();
	void sleep(unsigned int const &msec);
};
