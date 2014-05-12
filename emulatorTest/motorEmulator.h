#pragma once

#include "motorDevice.h"

namespace emulators
{
class Q_DECL_EXPORT MotorEmulator : public QObject
{
	Q_OBJECT
public:
	explicit MotorEmulator(MotorDevice *device);

public slots:
	/// Sets current motor power to specified value, 0 to stop motor.
	void setPower(int power);

	/// Returns currently set power of a motor.
	int power() const;
protected:
	MotorDevice *mDevice;
};
}

