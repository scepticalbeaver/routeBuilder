#pragma once

#include <QtCore/QTimer>
#include <QtCore/QVector>
#include <QtCore/QList>
#include <QtCore/QFile>

#include <QDebug>

#include "motorComplect.h"

class TrackStorage : public QObject
{
	Q_OBJECT
public:
	explicit TrackStorage(QVector<MotorComplect *> *array, QObject *parent = 0);
	~TrackStorage();

	static int const timeout = 20;

	QVector<MotorComplect *> *devices();

public slots:
	void startRecording();
	void stopRecording();

protected:
	QTimer *mWatcher;
	QList<Storage::DeviceExtension *> mDevices;
	QVector<MotorComplect *> *mMotorComplects;

	void initTimer();
	void clearHistory();

protected slots:
	void readEncoders();
};

