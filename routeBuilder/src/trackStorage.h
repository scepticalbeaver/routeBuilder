#pragma once

#include <QtCore/QFile>
#include <QtCore/QTimer>
#include <QtCore/QTime>
#include <QtCore/QRegExp>
#include <QtCore/QStringList>

#include <QDebug>

#include "motorComplect.h"

class TrackStorage : public QObject
{
	Q_OBJECT
public:
	explicit TrackStorage(QVector<MotorComplect *> *array, QObject *parent = 0);
	~TrackStorage();

	QVector<float>* motorTrace(int const &id, int const &trackFlow = 0) const;

	bool loadFromFile(QString const &filename, int const &intoTrackFlow);

public slots:
	void startRecording(int const &flowNumber = 0);
	void stopRecording();

protected:
	static int const timeout = 500; //ms
	static int const flowOffset = 100; // offset between trackflows
	static int const isNotRecordingAny = -1;
	float const epsilon = 20;

	QTimer mWatcher;
	QVector<MotorComplect *> *mMotorComplects;
	QMap<int, QVector<float> *> mTrackLog;
	bool isFirstCapture;
	int mRecordingFlow;

	void saveTraceToFile();
	void clearFlowTrackLog(int const &flowNum);
	void clearAllTrackLog();
	void createStorageFromComplect(int const &trackFlow);
	bool createStorage(QStringList const &motorIDs, int const &trackFlow);
	bool appendDataLine(QString const &line, int const &trackFlow);
	bool isInFlowId(int const &id, int const &flow) const;
	int idToFlowId(int const &id, int const &flow) const;
	int idFromFlowId(int const &flowId) const;
	QList<int> flowIdsInFlow(int const &flow) const;

protected slots:
	void trace();
};

