#include "trackStorage.h"

using namespace Storage;

TrackStorage::TrackStorage(QVector<MotorComplect *> *array, QObject *parent)
	: QObject(parent)
{
	foreach (MotorComplect *complect, *array)
	{
		mDevices << new DeviceExtension(complect);
	}

	initConnections();
}

TrackStorage::~TrackStorage()
{
}

void TrackStorage::addValue(QVector<float> &data, float value)
{
	data.append(value);
}

void TrackStorage::stopRecording()
{
	mWatcher.stop();
}

bool TrackStorage::startRecording()
{
	if (mDevices.size() == 0)
	{
		return false;
	}

	mWatcher.start(timeout);

	return true;
}

QList<DeviceExtension *> &TrackStorage::devices()
{
	return mDevices;
}

void TrackStorage::initConnections()
{
	connect(&mWatcher, SIGNAL(timeout()), SLOT(readEncoders()));
}

void TrackStorage::readEncoders()
{
	for(int i = 0; i < mDevices.count(); i++)
	{
		addValue(mDevices.at(i)->history, mDevices.at(i)->motor->readEncoder());
	}
}
