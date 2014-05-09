#include "trackStorage.h"

TrackStorage::TrackStorage(QVector<MotorComplect *> *array, QObject *parent)
	: QObject(parent)
	, mMotorComplects(array)
	, isFirstCapture(true)
{
	connect(&mWatcher, SIGNAL(timeout()), SLOT(trace()));
}

TrackStorage::~TrackStorage()
{
	foreach (QVector<float> *vector, mTrackLog.values())
	{
		delete vector;
	}
}

QVector<float>* TrackStorage::motorTrace(int const id) const
{
	return mTrackLog.value(id);
}

void TrackStorage::startRecording()
{
	isFirstCapture = true;
	clearTrackLog();

	mWatcher.start(timeout);
}

void TrackStorage::stopRecording()
{
	mWatcher.stop();
	saveTraceToFile();
}

void TrackStorage::saveTraceToFile()
{
	int const count = mTrackLog.size();
	QString filename = "mC-trace_" + QString::number(count) + "-wheels_"
			+ QTime::currentTime().toString("hh-mm-ss-zzz") + ".log";
	QFile file(filename);
	if (!file.open(QFile::WriteOnly))
	{
		qDebug() << "--could not create file";
		return;
	}

	QString firstLine = "devs\' count: " + QString::number(mTrackLog.size()) + "\tIDs:\t";
	foreach (int id, mTrackLog.keys())
	{
		firstLine += QString::number(id) + "\t";
	}
	firstLine += "\n";
	file.write(firstLine.toUtf8());

	int const dataLength = mTrackLog.values().first()->size();
	QString line("");
	for (int i = 0; i < dataLength; i++)
	{
		foreach (int id, mTrackLog.keys())
		{
			line += QString::number(mTrackLog.value(id)->at(i), 'f', 2) + "\t";
		}
		line += "\n";
		file.write(line.toUtf8());
		line = "";
	}
	file.flush();
	file.close();
	qDebug() << "Trace saved to file \"" << filename << "\" with datalength: " << dataLength;
}

void TrackStorage::clearTrackLog()
{
	qDeleteAll(mTrackLog.values());

	foreach (MotorComplect *motor, (*mMotorComplects))
	{
		mTrackLog.insert(motor->id(), new QVector<float>());
	}
}

void TrackStorage::trace()
{
	bool hasChanges = false;

	if (isFirstCapture)
	{
		hasChanges = true;
		isFirstCapture = false;
	}
	else
	{
		foreach (MotorComplect *motor, (*mMotorComplects))
		{
			hasChanges = hasChanges || (qAbs(motor->readEncoder() - mTrackLog.value(motor->id())->last()) > epsilon);
		}
	}

	if (!hasChanges)
	{
		return;
	}

	foreach (MotorComplect *motor, (*mMotorComplects))
	{
		mTrackLog.value(motor->id())->append(motor->readEncoder());
	}
}

