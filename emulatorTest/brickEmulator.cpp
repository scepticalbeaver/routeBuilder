#include "brickEmulator.h"

using namespace emulators;

BrickEmulator::BrickEmulator(int const &motorsCount)
	: mMotorDevicesCount(motorsCount)
{
	for (int i = 0; i < motorsCount; i++)
	{
		mMotorDevices << new MotorDevice(this);
		mPowerMotors.insert(QString("JM") + QString::number(i + 1), new MotorWrap(mMotorDevices.last()));
		mEncoders.insert(QString("JB") + QString::number(i + 1), new EncoderWrap(mMotorDevices.last()));
	}
}

MotorWrap *BrickEmulator::motor(QString const &port)
{
	return mPowerMotors.value(port, nullptr);
}

QStringList BrickEmulator::motorPorts(trikControl::Motor::Type type) const
{
	if (type == trikControl::Motor::powerMotor)
	{
		return mPowerMotors.keys();
	}
	else
	{
		return QStringList();
	}
}

QStringList BrickEmulator::encoderPorts() const
{
	return mEncoders.keys();
}

EncoderWrap *BrickEmulator::encoder(QString const &port)
{
	return mEncoders.value(port, nullptr);
}
