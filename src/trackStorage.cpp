#include "trackStorage.h"

TrackStorage::TrackStorage(QVector<MotorComplect *> *array, QObject *parent)
	: QObject(parent)
	, mWatcher(nullptr)
	, mMotorComplects(array)
{
	initTimer();
}

TrackStorage::~TrackStorage()
{
}

void TrackStorage::stopRecording()
{
	mWatcher->stop();

//	qDebug() << "history size: " << mDevices.last()->history.size();
	for(int i = 0; i < mMotorComplects->size(); i++)
	{
		mMotorComplects->at(i)->completeHistory();
	}
}

void TrackStorage::startRecording()
{
	initTimer();

	if (mMotorComplects->size() == 0)
	{
		qDebug() << "--no tracking device found";
		return;
	}
	clearHistory();

	mWatcher->start(timeout);
}

QVector<MotorComplect *> *TrackStorage::devices()
{
	return mMotorComplects;
}

void TrackStorage::initTimer()
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
	for(int i = 0; i < mMotorComplects->size(); i++)
	{
		mMotorComplects->at(i)->updateHistory();
	}
}

void TrackStorage::clearHistory()
{
	for (int i = 0; i < mMotorComplects->size(); i++)
	{
		mMotorComplects->at(i)->clearHistory();
	}
}
