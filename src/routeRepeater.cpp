#include "routeRepeater.h"

RouteRepeater::RouteRepeater(TrackStorage *storage, QObject *parent)
	: QObject(parent)
	, mStorage(storage)
	, mDevices(storage->devices())
	, mTimer(nullptr)
	, mDevicesCount(0)
{
}

RouteRepeater::~RouteRepeater()
{
	stopMotors();
}

void RouteRepeater::playback()
{
	for (int i = 0; i < mDevices->size(); i++)
	{
		connect(mDevices->at(i), SIGNAL(playbackDone()), SLOT(playbackStopped()), Qt::UniqueConnection);
	}
	initTimer();
	qDebug() << "--starting playback";
	for (int i = 0; i < mDevicesCount; i++)
	{
		mDevices->at(i)->startPlayback();
	}

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

void RouteRepeater::playbackStopped()
{
	if (!mTimer->isActive())
	{
		return;
	}
	qDebug() << "--playback stopped signal received";
	mTimer->stop();
	for (int i = 0; i < mDevices->size(); i++)
	{
		mDevices->at(i)->completePlayback();
	}
	stopMotors();
	emit playbackDone();
}
