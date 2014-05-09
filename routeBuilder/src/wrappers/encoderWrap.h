#pragma once

#include "../../../trikRuntime/trikControl/include/trikControl/encoder.h"
#include "../../emulatorTest/encoderEmulator.h"


//! @class EncoderWrap is a container for encoder of real or emulation type
class EncoderWrap : public QObject
{
	Q_OBJECT
public:
	explicit EncoderWrap(trikControl::Encoder *encoder);
	explicit EncoderWrap(emulators::EncoderEmulator *encoder);

	~EncoderWrap() {}
public slots:
	/// Returns current encoder reading.
	float read();

	/// Resets encoder by setting current reading to 0.
	void reset();

protected:
	bool const mHasRealDevice;
	trikControl::Encoder *mRealEncoder;
	emulators::EncoderEmulator *mEncoderEmulator;
};

