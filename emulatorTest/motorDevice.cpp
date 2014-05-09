#include "motorDevice.h"

using namespace emulators;

MotorDevice::MotorDevice(QObject *parent)
	: QObject(parent)
	, mPower(0)
	, mEncoderValue(0)
	, mDestSpeed(0)
	, mSpeed(0)
{
	connect(&mUpdater, SIGNAL(timeout()), SLOT(updateState()));
	mUpdater.start(this->timeout);
}

MotorDevice::~MotorDevice()
{
	mUpdater.stop();
}


void MotorDevice::resetEncoder()
{
	mEncoderValue = 0;
}

float MotorDevice::readEncoder() const
{
	return mEncoderValue;
}

void MotorDevice::setPower(int const &value)
{
	int const maxPower = 100;
	int newValue = qMin(qMax(value, -maxPower), maxPower);

	mDestSpeed = ((double)newValue / maxPower) * maxSpeed;

	if (!mUpdater.isActive())
	{
		mUpdater.start(this->timeout);
	}
}

int MotorDevice::power() const
{
	return mPower;
}

void MotorDevice::adjustSpeed()
{
	double const accPerTick = accelerate *  ((double)this->timeout / 1000);

	if (qAbs(mSpeed - mDestSpeed) < (accPerTick / 2))
	{
		mSpeed = mDestSpeed;
		return;
	}

	mSpeed += (mSpeed < mDestSpeed)? accPerTick : -accPerTick;
}

void MotorDevice::updateState()
{
	if (mSpeed != mDestSpeed)
	{
		adjustSpeed();
	}

	mEncoderValue += encPerRound * (mSpeed * ((double)this->timeout / 1000)) / roundLength();
}
