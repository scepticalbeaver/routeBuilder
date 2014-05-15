#include "trackStorage.h"
#include "constants.h"

TrackStorage::TrackStorage(QVector<MotorComplect *> *array, QObject *parent)
	: QObject(parent)
	, mMotorComplects(array)
	, isFirstCapture(true)
	, mRecordingFlow(isNotRecordingAny)
{
	connect(&mWatcher, SIGNAL(timeout()), SLOT(trace()));
}

TrackStorage::~TrackStorage()
{
	qDeleteAll(mTrackLog.values());
}

QVector<float>* TrackStorage::motorTrace(int const &id, int const &trackFlow) const
{
	return mTrackLog.value(idToFlowId(id, trackFlow));
}

bool TrackStorage::loadFromFile(QString const &filename, int const &intoTrackFlow)
{
	clearFlowTrackLog(intoTrackFlow);
	QFile file(filename);
	if (!file.open(QFile::ReadOnly))
	{
		qDebug() << "--could not read file " << filename;
		return false;
	}

	QString header(file.readLine());
	QRegExp digitsRX("(\\d+)");
	QStringList numbers;
	int pos = 0;

	while ((pos = digitsRX.indexIn(header, pos)) != -1)
	{
		numbers << digitsRX.cap(1);
		pos += digitsRX.matchedLength();
	}

	if ((numbers.first().toInt() != numbers.size() - 1))
	{
		qDebug() << "--File is corrupted " << filename;
		return false;
	}
	numbers.removeFirst();

	if (!createStorage(numbers, intoTrackFlow))
	{
		qDebug() << "--Motor IDs in file are corrupted " << filename;
		return false;
	}

	while (file.canReadLine())
	{
		if (!appendDataLine(file.readLine(), intoTrackFlow))
		{
			qDebug() << "--Encoders data is corrupted in file " << filename;
			return false;
		}
	}

	file.close();
	return true;
}

void TrackStorage::startRecording(int const &flowNumber)
{
	if (mRecordingFlow != isNotRecordingAny)
	{
		qDebug() << "--Recording already turned on!";
		return;
	}
	isFirstCapture = true;
	mRecordingFlow = flowNumber;
	clearFlowTrackLog(flowNumber);
	createStorageFromComplect(flowNumber);

	mWatcher.start(timeout);
}

void TrackStorage::stopRecording()
{
	mWatcher.stop();
	trace();
	saveTraceToFile();
	mRecordingFlow = isNotRecordingAny;
}

void TrackStorage::saveTraceToFile()
{
	QList<int> flowIdsList = flowIdsInFlow(mRecordingFlow);
	int const count = flowIdsList.size();
	QString filename = "trace-" + QString::number(mRecordingFlow) + "th-flow_on-" + QString::number(count) + "-wheels_"
			+ QTime::currentTime().toString("hh-mm-ss-zzz") + ".log";
	QFile file(filename);
	if (!file.open(QFile::WriteOnly))
	{
		qDebug() << "--could not create file " << filename;
		return;
	}


	QString firstLine = "devs\' count: " + QString::number(flowIdsList.size()) + "\tIDs:\t";
	foreach (int flowId, flowIdsList)
	{
		firstLine += QString::number(idFromFlowId(flowId)) + "\t";
	}
	firstLine += "\n";
	file.write(firstLine.toUtf8());

	int const dataLength = mTrackLog.value(flowIdsList.first())->size();
	QString line("");
	for (int i = 0; i < dataLength; i++)
	{
		foreach (int const &flowId, flowIdsList)
		{
			line += QString::number(mTrackLog.value(flowId)->at(i), 'f', 2) + "\t";
		}
		line += "\n";
		file.write(line.toUtf8());
		line = "";
	}
	file.flush();
	file.close();
	qDebug() << "--Trace saved to file " << filename << " with datalength: " << dataLength;
}

void TrackStorage::clearFlowTrackLog(int const &flowNum)
{
	foreach (int const &id, mTrackLog.keys())
	{
		if (isInFlowId(id, flowNum))
		{
			delete mTrackLog.value(id);
			mTrackLog.remove(id);
		}
	}
}

void TrackStorage::clearAllTrackLog()
{
	qDeleteAll(mTrackLog.values());
	mTrackLog.clear();
}

bool TrackStorage::createStorage(QStringList const &motorIDs, int const &trackFlow)
{
	int curID = 0;
	bool isGood = false;
	foreach (QString const &id, motorIDs)
	{
		curID = id.toInt(&isGood);
		if (!isGood)
		{
			clearFlowTrackLog(trackFlow);
			return false;
		}
		mTrackLog.insert(idToFlowId(curID, trackFlow), new QVector<float>());
	}
	return true;
}

bool TrackStorage::appendDataLine(QString const &line, int const &trackFlow)
{
	QRegExp digitsRX("\[\t\n]");
	QStringList values = line.split(digitsRX, QString::SkipEmptyParts);
	float curValue = 0;
	bool isGoodNum = false;
	foreach (int const &id, flowIdsInFlow(trackFlow))
	{
		curValue = values.takeFirst().toFloat(&isGoodNum);
		if (!isGoodNum)
		{
			clearFlowTrackLog(trackFlow);
			return false;
		}
		mTrackLog.value(idToFlowId(id, trackFlow))->append(curValue);
	}
	return true;
}

bool TrackStorage::isInFlowId(int const &id, int const &flow) const
{
	return (id >= flowOffset * flow && id < flowOffset * (flow + 1));
}

int TrackStorage::idToFlowId(int const &id, int const &flow) const
{
	return id + flowOffset * flow;
}

int TrackStorage::idFromFlowId(int const &flowId) const
{
	return flowId % flowOffset;
}

QList<int> TrackStorage::flowIdsInFlow(int const &flow) const
{
	QList<int> result;
	foreach (int const &id, mTrackLog.keys())
	{
		if (isInFlowId(id, flow))
		{
			result << id;
		}
	}
	return result;
}

void TrackStorage::createStorageFromComplect(int const &trackFlow)
{
	foreach (MotorComplect const *motor, (*mMotorComplects))
	{
		mTrackLog.insert(idToFlowId(motor->id(), trackFlow), new QVector<float>());
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
			hasChanges = hasChanges ||
					(qAbs(motor->readEncoder() - mTrackLog.value(idToFlowId(motor->id(), mRecordingFlow))->last())
					> epsilon);
		}
	}

	if (!hasChanges)
	{
		return;
	}

	foreach (MotorComplect *motor, (*mMotorComplects))
	{
		mTrackLog.value(idToFlowId(motor->id(), mRecordingFlow))->append(motor->readEncoder());
	}
}

