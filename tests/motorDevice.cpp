#include "motorDevice.h"

using namespace tests;

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
	qDebug() << mEncoderValue;
	return mEncoderValue;
}

void MotorDevice::setPower(int const &value)
{
	int const maxPower = 100;
	int newValue = qMin(qMax(value, -maxPower), maxPower);

	mDestSpeed = (newValue / maxPower) * maxSpeed;

	if (!mUpdater.isActive())
	{
		mUpdater.start(this->timeout);
	}
}

void MotorDevice::adjustSpeed()
{
	double const accPerTick = accelerate *  (this->timeout / 1000);

	if (qAbs(mSpeed - mDestSpeed) < (accPerTick / 2))
	{
		mSpeed = mDestSpeed;
		mUpdater.stop();
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

	mEncoderValue += encPerRound * (mSpeed * (this->timeout / 1000)) / roundLength();
}
