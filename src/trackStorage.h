#pragma once

#include <QtCore/QTimer>
#include <QtCore/QVector>
#include <QtCore/QList>
#include <QtCore/QFile>
#include <QtCore/QEventLoop>

#include <QDebug>

#include "motorComplect.h"

namespace Storage
{
struct DeviceExtension
{
	MotorComplect *motor;
	QVector <float> history;

	DeviceExtension(MotorComplect *motor)
	{
		this->motor = motor;
	}
};
}

class TrackStorage : public QObject
{
	Q_OBJECT
public:
	explicit TrackStorage(QVector<MotorComplect *> *array, QObject *parent = 0);
	~TrackStorage();

	static int const timeout = 20;
	static int const epsilon = 10;

	void loadDevices();
	QList<Storage::DeviceExtension *> &devices();

public slots:
	void startRecording();
	void stopRecording();

protected:
	QTimer *mWatcher;
	QList<Storage::DeviceExtension *> mDevices;
	QVector<MotorComplect *> *mPureComplects;

	void initTImer();
	void fetchDevices();
	void clearHistory();

protected slots:
	void readEncoders();
};

