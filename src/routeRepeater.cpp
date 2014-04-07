#include "routeRepeater.h"

using namespace Storage;

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
	mDevicesCount = mStorage->devices().count();
	mPeriod = 0;
	mLastPeriod = mStorage->devices().last()->history.size();
	for (int i = 0; i < mDevicesCount; i++)
	{
		mStorage->devices().at(i)->motor->resetEncoder();
	}

	mTimer.start(mStorage->timeout);
}

void RouteRepeater::adjust()
{
	if (mPeriod >= mLastPeriod)
	{
		mTimer.stop();
		return;
	}
	float correctValue = 0;
	float currentValue = 0;
	for (int i = 0; i < mDevicesCount; i++)
	{
		correctValue = mStorage->devices().at(i)->history.at(mPeriod++);
		currentValue = mStorage->devices().at(i)->motor->readEncoder();
		if (qAbs(correctValue - currentValue) < mStorage->epsilon)
		{
			return;
		}
		if (currentValue < correctValue)
		{
			mStorage->devices().at(i)->motor->increaseSpeed();
		}
		else
		{
			mStorage->devices().at(i)->motor->decreaseSpeed();
		}
	}
}
