#include "routeRepeater.h"

RouteRepeater::RouteRepeater(TrackStorage *storage, QObject *parent)
	: QObject(parent)
	, mStorage(storage)
	, mDevicesCount(0)
	, mPeriod(0)
	, mLastPeriod(0)
{
	connect(&mTimer, SIGNAL(timeout()), SLOT(adjust()));
}

void RouteRepeater::playback()
{
	mDevicesCount = mStorage->devices()->size();
	mLastPeriod = mStorage->routeData()->value(mDevicesCount - 1).size();
	foreach(MotorComplect &complect, mStorage->devices())
	{
		complect.resetEncoder();
	}

	mTimer.start(mStorage->timeout);
}

void RouteRepeater::adjust()
{
	if (mPeriod >= mLastPeriod)
	{
		mTimer.stop();
	}
	float correctValue = 0;
	float currentValue = 0;
	for (int i = 0; i < mDevicesCount; i++)
	{
		correctValue = mStorage->routeData()->value(i).at(mPeriod++);
		currentValue = mStorage->devices()->at(i).readEncoder();
		if (qAbs(correctValue - currentValue) < mStorage->epsilon)
		{
			return;
		}
		if (currentValue < correctValue)
		{
			mStorage->devices()->at(i).increaseSpeed();
		}
		else
		{
			mStorage->devices()->at(i).decreaseSpeed();
		}
	}
}
