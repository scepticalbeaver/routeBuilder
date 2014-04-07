#pragma once

#include "trikControl/brick.h"

#include <QDebug>

class MotorComplect
{
public:
	MotorComplect(trikControl::Motor *motor, trikControl::Encoder *motorEncoder);
	void setMotor(trikControl::Motor *motor);
	void setEncoder(trikControl::Encoder *motorEncoder);
	trikControl::Motor* motor();
	trikControl::Encoder* encoder();
	float readEncoder();
	void resetEncoder();

	void setMotorPower(int power);
	void setIncrement(int const &increment);
	void setReversed(bool const &isReversed);
	void increaseSpeed();
	void decreaseSpeed();

protected:
	trikControl::Motor *mMotor;
	trikControl::Encoder *mEncoder;
	int mPower;
	int mIncrement;
	bool mIsReversed;
};

