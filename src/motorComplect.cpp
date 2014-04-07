#include "motorComplect.h"

using namespace trikControl;

MotorComplect::MotorComplect(Motor *motor, Encoder *motorEncoder)
	: mMotor(motor)
	, mEncoder(motorEncoder)
	, mPower(0)
	, mIncrement(4)
	, mIsReversed(false)
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
	}
	return mEncoder->read();
}

void MotorComplect::resetEncoder()
{
	mEncoder->reset();
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
