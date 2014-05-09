#include "encoderEmulator.h"

using namespace emulators;

EncoderEmulator::EncoderEmulator(emulators::MotorDevice *device)
	: QObject(device)
	, mDevice(device)
{
}

float EncoderEmulator::read()
{
	return mDevice->readEncoder();
}

void EncoderEmulator::reset()
{
	mDevice->resetEncoder();
}
