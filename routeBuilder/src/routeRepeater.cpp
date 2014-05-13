#include "routeRepeater.h"

RouteRepeater::RouteRepeater(QVector<MotorComplect *> *complects, TrackStorage *storage, QObject *parent)
	: QObject(parent)
	, mStorage(storage)
	, mSharedComplects(complects)
	, mHistoryPointer(0)
	, mPlaybackFlow(keywords::TrackingFlows::mainTrackingFlow())
{
	connect(&mTimer, SIGNAL(timeout()), SLOT(adjustMotors()));
}

void RouteRepeater::playback(int const &trackFlow)
{
	qDebug() << "--starting playback";
	mHistoryPointer = 0;
	fetchMotors();
	mHistorySize = mStorage->motorTrace(mMotorComplects.keys().first(), trackFlow)->size();
	foreach (MotorComplect *motor, mMotorComplects.values())
	{
		motor->resetEncoder();
	}

	mStorage->startRecording(keywords::TrackingFlows::alternativeFlow());

	mTimer.start(timeout);
}

void RouteRepeater::stopMotors()
{
	foreach (MotorComplect *motor, mMotorComplects.values())
	{
		motor->setMotorPower(0);
	}
}

void RouteRepeater::moveToNextPoint(QMap<MotorComplect*, float> &encoderValues)
{
	bool hasReachedFinish = mHistoryPointer == mHistorySize - 1;

	int nextPosition = mHistorySize;
	foreach (MotorComplect *motor, encoderValues.keys())
	{
		int motorId = motor->id();

		bool isReversed = isCurrentPointBackwards(motorId);

		if (hasReachedFinish)
		{
			hasReachedFinish = hasReachedPosition(encoderValues.value(motor), valueAtTimePos(motorId, mHistoryPointer)
					, isReversed);
		}

		int localNextPos = mHistoryPointer;
		while (localNextPos < mHistorySize - 1
				&& hasReachedPosition(
						encoderValues.value(motor)
						, valueAtTimePos(motorId, localNextPos)
						, isNegativePointsDiff(motorId, (localNextPos)? localNextPos - 1 : 0, localNextPos)
						))
		{
			localNextPos++;
		}
		nextPosition = qMin(localNextPos, nextPosition);
	}
	mHistoryPointer = qMin(nextPosition, mHistorySize - 1);

	if (hasReachedFinish)
	{
		playbackStopped();
	}
}

bool RouteRepeater::isNegativePointsDiff(int const &id, int const &from, int const &to)
{
	return mStorage->motorTrace(id, mPlaybackFlow)->at(to) - mStorage->motorTrace(id, mPlaybackFlow)->at(from) < 0;
}

bool RouteRepeater::isNextPointBackwards(int const &id)
{
	int after = (mHistoryPointer == mHistorySize - 1)? mHistoryPointer : mHistoryPointer + 1;
	int before = after - 1;

	return isNegativePointsDiff(id, before, after);
}

bool RouteRepeater::isCurrentPointBackwards(int const &id)
{
	int after = mHistoryPointer;
	int before = (after == 0)? after : after - 1;

	return isNegativePointsDiff(id, before, after);
}

bool RouteRepeater::hasReverseVelocity(int const &id, float const &currentValue)
{
	return mStorage->motorTrace(id, mPlaybackFlow)->at(mHistoryPointer) - currentValue < 0;
}

bool RouteRepeater::hasReachedPosition(float const &currentPos, float const finPos, bool const &isReversed)
{
	//! @todo take epsilon from mapper, which knows about robot encoder params
	float const epsilon = 20;
	bool isInLocality = qAbs(currentPos - finPos) < epsilon;
	return (!isReversed && (currentPos >= finPos)) || (isReversed && (currentPos <= finPos)) || isInLocality;
}

float RouteRepeater::valueAtTimePos(int const &id, int const &pointer)
{
	return mStorage->motorTrace(id, mPlaybackFlow)->at(pointer);
}

QMap<MotorComplect *, float> RouteRepeater::fetchCurrEncoderValues()
{
	QMap<MotorComplect *, float> result;
	foreach (MotorComplect *motor, mMotorComplects.values())
	{
		result.insert(motor, motor->readEncoder());
	}
	return result;
}

QMap<MotorComplect *, float> RouteRepeater::blockExcessMotors(QMap<MotorComplect *, float> &encoderValues)
{
	QMap<MotorComplect *, float> result;
	foreach(MotorComplect *motor, mMotorComplects.values())
	{
		if (hasReachedPosition(
				encoderValues.value(motor)
				, valueAtTimePos(motor->id(), mHistoryPointer)
				, hasReverseVelocity(motor->id(), encoderValues.value(motor))
				))
		{
			result.insert(motor, isBlockedMotor);
		}
	}
	return result;
}

void RouteRepeater::adjustMotors()
{
	QMap<MotorComplect *, float> currentEncoderValue = fetchCurrEncoderValues();

	moveToNextPoint(currentEncoderValue);

	QMap<MotorComplect *, float> difference = blockExcessMotors(currentEncoderValue);
	float maxDifference = 0.5;

	//calc length to target(difference) for each wheel
	foreach (MotorComplect *motor, mMotorComplects.values())
	{
		if (difference.value(motor, 0) == isBlockedMotor)
		{
			difference.insert(motor, 0);
			continue;
		}

		float curDiff = qAbs(mStorage->motorTrace(motor->id(), mPlaybackFlow)->at(mHistoryPointer)
				- currentEncoderValue.value(motor));
		if (curDiff >= maxDifference)
		{
			maxDifference = curDiff;
		}
		difference.insert(motor, curDiff);
	}

	//calc proper velocity using difference
	foreach (MotorComplect *motor, mMotorComplects.values())
	{
		float properVelocity = recommendVelocity * (difference.value(motor) / maxDifference);
		properVelocity *= (hasReverseVelocity(motor->id(), currentEncoderValue.value(motor)))? -1.0 : 1.0;
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
	stopMotors();
	mStorage->stopRecording();

	qDebug() << "--playback stopped";
	qDebug() << "--motors stopped at value:";
	foreach (MotorComplect *motor, mMotorComplects.values())
	{
		qDebug() << motor->id() << "\t" << motor->readEncoder();
	}

	emit playbackDone();
}

void RouteRepeater::fetchMotors()
{
	mMotorComplects.clear();
	foreach (MotorComplect *complect, (*mSharedComplects))
	{
		mMotorComplects.insert(complect->id(), complect);
	}
}
