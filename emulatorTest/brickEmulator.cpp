#include "brickEmulator.h"

using namespace emulators;

BrickEmulator::BrickEmulator(int const &motorsCount)
	: mMotorDevicesCount(motorsCount)
{
	for (int i = 0; i < motorsCount; i++)
	{
		mMotorDevices << new MotorDevice(this);
		mPowerMotors.insert(QString("JM") + QString::number(i + 1), new MotorEmulator(mMotorDevices.last()));
		mEncoders.insert(QString("JB") + QString::number(i + 1), new EncoderEmulator(mMotorDevices.last()));
	}
}

MotorEmulator* BrickEmulator::motor(QString const &port)
{
	return mPowerMotors.value(port, nullptr);
}

QStringList BrickEmulator::motorPorts() const
{
	return mPowerMotors.keys();
}

QStringList BrickEmulator::encoderPorts() const
{
	return mEncoders.keys();
}

EncoderEmulator* BrickEmulator::encoder(QString const &port)
{
	return mEncoders.value(port, nullptr);
}
