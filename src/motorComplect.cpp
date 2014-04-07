#include "motorComplect.h"

MotorComplect::MotorComplect(Motor *motor, Encoder *motorEncoder)
	: mPower(0)
	, mIncrement(10)
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
	return mEncoder->read();
}

void MotorComplect::resetEncoder()
{
	mEncoder->reset();
}

void MotorComplect::setMotorPower(int const &power)
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

void MotorComplect::increaseSpeed()
{
	setMotorPower(mPower + mIncrement);
}

void MotorComplect::decreaseSpeed()
{
	setMotorPower(mPower - mIncrement);
}
