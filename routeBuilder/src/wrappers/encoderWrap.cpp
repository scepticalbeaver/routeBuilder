#include "encoderWrap.h"

EncoderWrap::EncoderWrap(trikControl::Encoder *encoder)
	: QObject(encoder)
	, mHasRealDevice(true)
	, mRealEncoder(encoder)
	, mEncoderEmulator(nullptr)
{
}

EncoderWrap::EncoderWrap(emulators::EncoderEmulator *encoder)
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
		return mEncoderEmulator->read();
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
		mEncoderEmulator->reset();
	}
}

