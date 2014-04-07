#include "trackStorage.h"

using namespace Storage;

TrackStorage::TrackStorage(QVector<MotorComplect *> *array, QObject *parent)
	: QObject(parent)
	, mWatcher(nullptr)
	, mPureComplects(array)
{
	initTImer();
}

TrackStorage::~TrackStorage()
{
}

void TrackStorage::stopRecording()
{
	mWatcher->stop();
	qDebug() << "history size: " << mDevices.last()->history.size();
}

void TrackStorage::startRecording()
{
	initTImer();
	qDebug() << "--storage, start recording in thread " << this->thread();
	fetchDevices();
	if (mDevices.size() == 0)
	{
		qDebug() << "--zero device count";
		return;
	}
	clearHistory();
	mWatcher->start(timeout);

	qDebug() << "--recording timer started";
}

QList<DeviceExtension *> &TrackStorage::devices()
{
	return mDevices;
}

void TrackStorage::initTImer()
{
	if (mWatcher != nullptr)
	{
		return;
	}
	mWatcher = new QTimer(this);
	connect(mWatcher, SIGNAL(timeout()), this, SLOT(readEncoders()));
}

void TrackStorage::readEncoders()
{
	//qDebug() << "reading encoders";
	for(int i = 0; i < mDevices.count(); i++)
	{
		mDevices.at(i)->history.append(mDevices.at(i)->motor->readEncoder());
		//qDebug() << "--encoder x" << i << " : " << mDevices.at(i)->motor->readEncoder();
	}
}

void TrackStorage::fetchDevices()
{
	mDevices.clear();
	for (int i = 0; i < mPureComplects->size(); i++)
	{
		mDevices << new DeviceExtension(mPureComplects->at(i));
	}
}

void TrackStorage::clearHistory()
{
	for (int i = 0; i < mDevices.size(); i++)
	{
		mDevices.at(i)->history.clear();
	}
}
