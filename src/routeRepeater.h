#pragma once

#include <QDebug>

#include "trackStorage.h"

class RouteRepeater : public QObject
{
	Q_OBJECT
public:
	explicit RouteRepeater(TrackStorage *storage, QObject *parent = 0);

	void playback();

protected:
	TrackStorage *mStorage;
	QTimer *mTimer;
	int mDevicesCount;
	int mPeriod;
	int mLastPeriod;

	void initTimer();
	void stopMotors();

protected slots:
	void adjust();
};

