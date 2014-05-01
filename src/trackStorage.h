#pragma once

#include <QtCore/QMap>
#include <QtCore/QFile>
#include <QtCore/QTimer>
#include <QtCore/QVector>

#include <QDebug>

#include "motorComplect.h"

class TrackStorage : public QObject
{
	Q_OBJECT
public:
	explicit TrackStorage(QVector<MotorComplect *> *array, QObject *parent = 0);
	~TrackStorage();

	QVector<float>* const motorTrace(int const id) const;




public slots:
	void startRecording();
	void stopRecording();

protected:
	static int const timeout = 500; //ms
	static float const epsilon = 10;

	QTimer mWatcher;
	QVector<MotorComplect *> *mMotorComplects;
	QMap<int, QVector<float> *> mTrackLog;
	bool isFirstCapture;

	void saveTraceToFile();
	void clearTrackLog();

protected slots:
	void trace();


};

