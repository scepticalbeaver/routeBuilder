#include "routeRepeater.h"

using namespace Storage;

RouteRepeater::RouteRepeater(TrackStorage *storage, QObject *parent)
	: QObject(parent)
	, mStorage(storage)
	, mTimer(nullptr)
	, mDevicesCount(0)
	, mPeriod(0)
	, mLastPeriod(0)
{
}

void RouteRepeater::playback()
{
	initTimer();
	qDebug() << "--starting playback";
	mDevicesCount = mStorage->devices().size();

	mPeriod = 0;
	mLastPeriod = mStorage->devices().last()->history.size();

	qDebug() << "--history count " << mLastPeriod;

	mTimer->start(mStorage->timeout);
}

void RouteRepeater::initTimer()
{
	if (mTimer != nullptr)
	{
		return;
	}
	mTimer = new QTimer(this);
	connect(mTimer, SIGNAL(timeout()), SLOT(adjust()));

}

void RouteRepeater::stopMotors()
{
	for (int i = 0; i < mDevicesCount; i++)
	{
		mStorage->devices().at(i)->motor->setMotorPower(0);
	}
}

void RouteRepeater::adjust()
{
	if (mPeriod >= mLastPeriod)
	{
		mTimer->stop();
		stopMotors();
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
		qDebug() << "curr :: correct" << currentValue << " " << correctValue;
		if (currentValue < correctValue)
		{
			mStorage->devices().at(i)->motor->decreaseSpeed();
		}
		else
		{
			mStorage->devices().at(i)->motor->increaseSpeed();
		}
	}
}
