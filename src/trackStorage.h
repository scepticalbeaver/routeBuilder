#pragma once

#include <QtCore/QTimer>
#include <QtCore/QVector>
#include <QtCore/QList>
#include <QtCore/QFile>

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
	explicit TrackStorage(QVector<MotorComplect *> *list, QObject *parent);
	~TrackStorage();

	static int const timeout = 100;
	static int const epsilon = 20;
	bool startRecording();

	QList<Storage::DeviceExtension *> &devices();

public slots:
	void stopRecording();

protected:
	QTimer mWatcher;
	QList<Storage::DeviceExtension *> mDevices;

	void initConnections();
	void addValue(QVector<float> &data, float value);

protected slots:
	void readEncoders();
};

