#include "motorEmulator.h"

using namespace emulators;

MotorEmulator::MotorEmulator(emulators::MotorDevice *device)
	: QObject(device)
	, mDevice(device)
{
}

void MotorEmulator::setPower(int power)
{
	mDevice->setPower(power);
}

int MotorEmulator::power() const
{
	return mDevice->power();
}
