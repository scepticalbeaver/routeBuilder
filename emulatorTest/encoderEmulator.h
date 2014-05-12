#pragma once

#include "motorDevice.h"

namespace emulators
{
class Q_DECL_EXPORT EncoderEmulator : public QObject
{
	Q_OBJECT
public:
	explicit EncoderEmulator(MotorDevice *device);

public slots:
	/// Returns current encoder reading.
	float read();

	/// Resets encoder by setting current reading to 0.
	void reset();
protected:
	MotorDevice *mDevice;
};
}
