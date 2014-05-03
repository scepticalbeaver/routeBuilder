#include "motorComplect.h"

using namespace trikControl;

MotorComplect::MotorComplect(int const &complectID, Motor *motor, Encoder *motorEncoder, bool const &isReversed)
	: mMotor(motor)
	, mEncoder(motorEncoder)
	, mPower(0)
	, mIncrement(5)
	, mID(complectID)
	, mIsReversed(isReversed)
	, mIsMotorBlocked(false)
{
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

void MotorComplect::keepSpeed(float const &metersPerSecond)
{
	int const expPower = 100;
	float const expVelocity = 1.1;  //meters per second

	float properPower = expPower * (metersPerSecond / expVelocity);
	properPower = qMin(properPower, 100.0f);
	setMotorPower(int(properPower));
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
