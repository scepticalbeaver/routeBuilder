#pragma once

#include <QtCore/QTimer>
#include <QtCore/QVector>
#include <QtCore/QPair>
#include <QtCore/QFile>

#include "motorComplect.h"

class TrackStorage : public QObject
{
	Q_OBJECT
public:
	explicit TrackStorage(QVector<MotorComplect> *list, QObject *parent);
	~TrackStorage();

	static int const timeout = 100;
	static int const epsilon = 20;
	bool startRecording();

	QMap<int, QVector> *routeData();
	QVector<MotorComplect> *devices();

public slots:
	void stopRecording();

protected:
	QTimer mWatcher;
	QMap<int, QVector<float>> mRouteData;
	QVector<MotorComplect> *mDevices;

	void initConnections();
	void addValue(QVector<float> &data, float const &value);

protected slots:
	void readEncoders();
};

