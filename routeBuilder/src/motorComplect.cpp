#include "motorComplect.h"

using namespace trikControl;

MotorComplect::MotorComplect(int const &complectID, MotorWrap *motor, EncoderWrap *motorEncoder, bool const &isReversed)
	: mMotor(motor)
	, mEncoder(motorEncoder)
	, mPower(0)
	, mIncrement(1)
	, mID(complectID)
	, mIsReversed(isReversed)
	, mIsMotorBlocked(false)
	, mPrevEncoderDiff(0)
	, mPrevEncoderValue(0)
	, mProperEncDiff(0)
{
	connect(&mSpeedKeeper,SIGNAL(timeout()), SLOT(keepSpeed()));
}

void MotorComplect::setOrigins(QString const &motorPort, QString const &encoderPort)
{
	mMotorPort = motorPort;
	mEncoderPort = encoderPort;
}

QString MotorComplect::motorPort() const
{
	return mMotorPort;
}

QString MotorComplect::encoderPort() const
{
	return mEncoderPort;
}

int MotorComplect::id() const
{
	return mID;
}

float MotorComplect::readEncoder()
{
	return mEncoder->read() * ((mIsReversed)? -1 : 1);
}

void MotorComplect::resetEncoder()
{
	mEncoder->reset();
	mPrevEncoderDiff = 0;
}

void MotorComplect::setMotorPower(int power)
{
	if (mIsMotorBlocked)
	{
		return;
	}
	power = qMin(power, 100);
	power = qMax(power, -100);
	power = power * ((mIsReversed)? -1 : 1);
	mMotor->setPower(power);
	mPower = power;
}

void MotorComplect::setSpeed(float const &metersPerSecond)
{
	int const maxPower = 100;
	float const expVelocity = 1.1;  //meters per second
/*
	mProperEncDiff = (metersPerSecond * (this->timeout / 1000)) / (pi * wheelDiameter) * encodersPerRound;

	float const encMaxDiff = maxDiffPerMS * this->timeout;

	float properPower = (mProperEncDiff / encMaxDiff) * maxPower;
*/
	float properPower = maxPower * (metersPerSecond / expVelocity);
	properPower = qMin(properPower, 100.0f);
	setMotorPower(int(properPower));



	if (qAbs(metersPerSecond) >= 0.001)
	{
		mSpeedKeeper.start(this->timeout);
	}
}

void MotorComplect::setIncrement(int const &increment)
{
	mIncrement = increment;
}

bool MotorComplect::isReversed() const
{
	return mIsReversed;
}

void MotorComplect::increaseSpeed()
{
	setMotorPower(mPower +  mIncrement);
}

void MotorComplect::decreaseSpeed()
{
	setMotorPower(mPower - mIncrement);
}

bool MotorComplect::isStable(float const &curEncoder) const
{
	float const stablePercent = 0.17;
	float const diffPercent = qAbs(1 - (curEncoder / mPrevEncoderDiff));
	return (diffPercent <= stablePercent);
}

void MotorComplect::keepSpeed()
{
	float const eps = 1;
	float const encoderValue = mEncoder->read();
	float const dEnc = encoderValue - mPrevEncoderValue;
	if (isStable(dEnc))
	{
		if (dEnc - mProperEncDiff < eps)
		{
			increaseSpeed();
		}
		if (dEnc - mProperEncDiff > eps)
		{
			decreaseSpeed();
		}
	}

	mPrevEncoderValue = encoderValue;
	mPrevEncoderDiff = dEnc;
}
