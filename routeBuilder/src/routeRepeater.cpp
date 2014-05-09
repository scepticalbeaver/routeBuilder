#include "routeRepeater.h"

RouteRepeater::RouteRepeater(QVector<MotorComplect *> *complects, TrackStorage *storage, QObject *parent)
	: QObject(parent)
	, mStorage(storage)
	, mHistoryPointer(0)
{
	foreach (MotorComplect *motor, (*complects))
	{
		mMotorComplects->insert(motor->id(), motor);
	}
	connect(&mTimer, SIGNAL(timeout()), SLOT(adjustMotors()));
}

RouteRepeater::~RouteRepeater()
{
	stopMotors();
}

void RouteRepeater::playback()
{
	qDebug() << "--starting playback";
	mHistoryPointer = 0;
	mHistorySize = mStorage->motorTrace(mMotorComplects->keys().first())->size();
	foreach (MotorComplect *motor, mMotorComplects->values())
	{
		motor->resetEncoder();
	}
	mTimer.start(timeout);
}

void RouteRepeater::stopMotors()
{
	foreach (MotorComplect *motor, mMotorComplects->values())
	{
		motor->setMotorPower(0);
	}
}

void RouteRepeater::moveToNextPoint()
{
	bool hasReachedFinish = mHistoryPointer == mHistorySize - 1;
	int nextPosition = mHistoryPointer;
	float curValue = 0;
	int motorID = 0;
	foreach (MotorComplect *motor, mMotorComplects->values())
	{
		motorID = motor->id();
		curValue = motor->readEncoder();
		bool isReversed = hasReverseVelocity(motorID);
		if (hasReachedFinish)
		{
			hasReachedFinish = hasReachedPosition(curValue, valueAtTimePos(motorID, mHistoryPointer), isReversed);
		}
		while (hasReachedPosition(curValue, valueAtTimePos(motorID, nextPosition), isReversed))
		{
			if (++nextPosition >= mHistorySize - 1)
			{
				break;
			}
		}
	}
	mHistoryPointer = qMin(nextPosition, mHistorySize - 1);

	if (hasReachedFinish)
	{
		playbackStopped();
	}
}

bool RouteRepeater::hasReverseVelocity(int const id)
{
	int before = 0;
	int after = 0;
	if (mHistoryPointer < mHistorySize - 1)
	{
		before = mHistoryPointer;
		after = before + 1;
	}
	else
	{
		before = mHistoryPointer - 1;
		after = mHistoryPointer;
	}

	return mStorage->motorTrace(id)->at(after) - mStorage->motorTrace(id)->at(before) < 0;
}

bool RouteRepeater::hasReachedPosition(float const &currentPos, float const finPos, bool const &isReversed)
{
	return (!isReversed && (currentPos >= finPos)) || (isReversed && (currentPos <= finPos));
}

float RouteRepeater::valueAtTimePos(const int &id, const int &pointer)
{
	return mStorage->motorTrace(id)->at(pointer);
}

void RouteRepeater::adjustMotors()
{
	moveToNextPoint();

	QMap<int, float> difference;
	float maxDifference = 0.5;

	float curDiff = 0;
	float curValue = 0;
	foreach (MotorComplect *motor, mMotorComplects->values())
	{
		curValue = motor->readEncoder();
		curDiff = qAbs(mStorage->motorTrace(motor->id())->at(mHistoryPointer) - curValue);
		if (curDiff >= maxDifference)
		{
			maxDifference = curDiff;
		}
		difference.insert(motor->id(), curDiff);
	}

	float properVelocity = 0;
	foreach (MotorComplect *motor, mMotorComplects->values())
	{
		properVelocity = recommendVelocity * (difference.value(motor->id()) / maxDifference);
		properVelocity = properVelocity * (hasReverseVelocity(motor->id()))? -1 : 1;
		motor->setSpeed(properVelocity);
	}
}

void RouteRepeater::playbackStopped()
{
	if (!mTimer.isActive())
	{
		return;
	}
	mTimer.stop();
	qDebug() << "--playback stopped";

	stopMotors();
	emit playbackDone();
}
