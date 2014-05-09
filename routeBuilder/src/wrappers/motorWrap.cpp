#include "motorWrap.h"

MotorWrap::MotorWrap(trikControl::Motor *motor)
	: QObject(motor)
	, mHasRealDevice(true)
	, mRealMotor(motor)
	, mMotorEmulator(nullptr)
{
}

MotorWrap::MotorWrap(emulators::MotorEmulator *motor)
	: QObject(motor)
	, mHasRealDevice(false)
	, mRealMotor(nullptr)
	, mMotorEmulator(motor)
{
}

void MotorWrap::setPower(int power)
{
	if (mHasRealDevice)
	{
		mRealMotor->setPower(power);
	}
	else
	{
		mMotorEmulator->setPower(power);
	}
}

int MotorWrap::power() const
{
	if (mHasRealDevice)
	{
		return mRealMotor->power();
	}
	else
	{
		return mMotorEmulator->power();
	}
}



