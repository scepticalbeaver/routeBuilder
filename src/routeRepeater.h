#pragma once

#include "trackStorage.h"

class RouteRepeater : public QObject
{
	Q_OBJECT
public:
	explicit RouteRepeater(TrackStorage *storage, QObject *parent);

	void playback();

protected:
	TrackStorage *mStorage;
	QTimer mTimer;
	int mDevicesCount;
	int mPeriod;
	int mLastPeriod;

protected slots:
	void adjust();
};

