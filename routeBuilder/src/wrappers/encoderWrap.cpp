#include "encoderWrap.h"

EncoderWrap::EncoderWrap(trikControl::Encoder *encoder)
	: QObject(encoder)
	, mHasRealDevice(true)
	, mRealEncoder(encoder)
	, mEncoderEmulator(nullptr)
{
}

EncoderWrap::EncoderWrap(emulators::MotorDevice *encoder)
	: QObject(encoder)
	, mHasRealDevice(false)
	, mRealEncoder(nullptr)
	, mEncoderEmulator(encoder)
{
}

float EncoderWrap::read()
{
	if (mHasRealDevice)
	{
		return mRealEncoder->read();
	}
	else
	{
		return mEncoderEmulator->readEncoder();
	}
}

void EncoderWrap::reset()
{
	if (mHasRealDevice)
	{
		mRealEncoder->reset();
	}
	else
	{
		mEncoderEmulator->resetEncoder();
	}
}

