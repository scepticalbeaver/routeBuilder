#pragma once

#include "../../../trikRuntime/trikControl/include/trikControl/motor.h"
#include "../../../emulatorTest/motorDevice.h"

//! @class MotorWrap is a container for motor of real or emulation type
class  MotorWrap : public QObject
{
	Q_OBJECT
public:
	explicit MotorWrap(trikControl::Motor *motor);
	explicit MotorWrap(emulators::MotorDevice *motor);

	~MotorWrap() {}

public slots:
	/// Sets current motor power to specified value, 0 to stop motor.
	void setPower(int power);

	/// Returns currently set power of a motor.
	int power() const;

protected:
	bool const mHasRealDevice;
	trikControl::Motor *mRealMotor;
	emulators::MotorDevice *mMotorEmulator;
};

