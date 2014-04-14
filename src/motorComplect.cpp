#include "motorComplect.h"

using namespace trikControl;

MotorComplect::MotorComplect(Motor *motor, Encoder *motorEncoder)
	: mMotor(motor)
	, mEncoder(motorEncoder)
	, mPower(0)
	, mIncrement(4)
	, mIsReversed(false)
	, mLatestEncoderVal(0)
	, mHistoryPointer(0)
{
}

void MotorComplect::setMotor(trikControl::Motor *motor)
{
	mMotor = motor;
}

void MotorComplect::setEncoder(trikControl::Encoder *motorEncoder)
{
	mEncoder = motorEncoder;
}

trikControl::Motor* MotorComplect::motor()
{
	return mMotor;
}

trikControl::Encoder* MotorComplect::encoder()
{
	return mEncoder;
}

float MotorComplect::readEncoder()
{
	if (mEncoder == nullptr)
	{
		qDebug() << "nullptr encoder requested";
		return -1;
	}
	return mEncoder->read();
}

void MotorComplect::resetEncoder()
{
	mEncoder->reset();
}

void MotorComplect::clearHistory()
{
	mHistory.clear();
	mHistoryPointer = 0;
}

void MotorComplect::setMotorPower(int power)
{
	power = qMin(power, 100);
	power = qMax(power, -100);
	mMotor->setPower(power);
	mPower = power;
}

void MotorComplect::setIncrement(int const &increment)
{
	mIncrement = increment;
}

void MotorComplect::setReversed(bool const &isReversed)
{
	mIsReversed = isReversed;
}

void MotorComplect::increaseSpeed()
{
	setMotorPower(mPower + ((mIsReversed)? mIncrement : -mIncrement));
}

void MotorComplect::decreaseSpeed()
{
	setMotorPower(mPower + ((mIsReversed)? -mIncrement : mIncrement));
}

void MotorComplect::updateHistory()
{
	float const current = mEncoder->read();
	mHistory.append(mLatestEncoderVal - current);
	mLatestEncoderVal = current;
}

void MotorComplect::startPlayback()
{
	mHistoryPointer = 0;
	for (int i = 0; i < forecastRange + 1; i++)
	{
		mHistory.append(0);
	}
}

void MotorComplect::adjustSpeed()
{
	float const current = mEncoder->read();
	float const diff = current - mLatestEncoderVal;
	mLatestEncoderVal = current;
	float const nextDiff = forecastNextValue();

	//qDebug() << "curr :: correct" << currentValue << " " << correctValue;
	if (qAbs(diff - nextDiff) < epsilon)
	{
		return;
	}

	if (diff < nextDiff)
	{
		increaseSpeed();
	}
	else
	{
		decreaseSpeed();
	}
}

float MotorComplect::forecastNextValue()
{
	int const histrorySize = mHistory.size();
	if (mHistoryPointer >= histrorySize)
	{
		emit playbackDone();
		return mHistory.last();
	}

	while (mHistoryPointer >= histrorySize - lookForwardDistance)
	{
		lookForwardDistance--;
	}

	float result = 0;
	int totalWeight = 0;
	for (int i = 0; i < lookForwardDistance; i++)
	{
		result += mHistory.at(mHistoryPointer + i) * (i + 1);
		totalWeight += i + 1;
	}
	mHistoryPointer++;
	return result / totalWeight;
}
