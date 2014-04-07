#pragma once

#include <QtCore/QObject>
#include <QtCore/QThread>

#include <iostream>

#include "routeController.h"

class InteractiveCommander : public QObject
{
	Q_OBJECT
public:
	InteractiveCommander(QThread *guiThread, QObject *parent = 0);
	~InteractiveCommander();

signals:
	void trackingRequested();
	void stopTrackingRequested();
	void turnMotorsRequested(bool);
	void initDevicesRequest();
	void playbackRequested();

public slots:
	void start();

protected:
	QThread *mGuiThread;
	RouteController *mRouteController;
	QThread *mAlternativeThread;

	void initConnections();
	void loopRound();
	void printRoundMsg();
	void initDevicesSignal();
	void launchTracking();
	void completeTracking();
	void startPlayback();
	void switchMotors(bool const willTurnOn);

protected slots:
	void routeActionFinished(bool const successfully);

};
