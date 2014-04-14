#include "routeRepeater.h"

using namespace Storage;

RouteRepeater::RouteRepeater(TrackStorage *storage, QObject *parent)
	: QObject(parent)
	, mStorage(storage)
	, mDevices(storage->devices())
	, mTimer(nullptr)
	, mDevicesCount(0)
{
}

void RouteRepeater::playback()
{
	initTimer();
	qDebug() << "--starting playback";

	mTimer->start(mStorage->timeout);
}

void RouteRepeater::initTimer()
{
	if (mTimer != nullptr)
	{
		return;
	}
	mTimer = new QTimer(this);
	connect(mTimer, SIGNAL(timeout()), SLOT(adjustMotors()));

}

void RouteRepeater::stopMotors()
{
	for (int i = 0; i < mDevicesCount; i++)
	{
		mDevices->at(i)->setMotorPower(0);
	}
}

void RouteRepeater::adjustMotors()
{
	for (int i = 0; i < mDevices->size(); i++)
	{
		mDevices->at(i)->adjustSpeed();
	}
}
