#include "trackStorage.h"

TrackStorage::TrackStorage(QList<MotorComplect> *list, QObject *parent)
	: QObject(parent)
	, mDevices(list)
{
	initConnections();
}

TrackStorage::~TrackStorage()
{

}

void TrackStorage::addValue(QVector<float> &data, float const &value)
{
	data.append(value);
}

void TrackStorage::stopRecording()
{
	mWatcher.stop();
}

bool TrackStorage::startRecording()
{
	if (mDevices == nullptr || mDevices->count() = 0)
	{
		return false;
	}

	mWatcher.start(timeout);

	return true;
}

QMap<int, QVector>* TrackStorage::routeData()
{
	return &mRouteData;
}

QVector<MotorComplect> *TrackStorage::devices()
{
	return mDevices;
}

void TrackStorage::initConnections()
{
	connect(&mWatcher, SIGNAL(timeout()), SLOT(readEncoders()));
}

void TrackStorage::readEncoders()
{
	for(int i = 0; i < mDevices->count(); i++)
	{
		addValue(mRouteData.value(i, QVector<float>()), mDevices->at(i).readEncoder());
	}
}
