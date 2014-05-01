#pragma once

#include <QDebug>

#include "trikControl/brick.h"

//! @class MotorComplect is a conformity of Motor and Encoder
class MotorComplect : public QObject
{
	Q_OBJECT
public:
	MotorComplect(trikControl::Motor *motor, trikControl::Encoder *motorEncoder, int const &complectID);

	//! @return unique identificator of motor complect
	int id() const;

	void setMotor(trikControl::Motor *motor);
	trikControl::Motor* motor();

	void setEncoder(trikControl::Encoder *motorEncoder);
	trikControl::Encoder* encoder();

	void setReversed(bool const &isReversed);
	bool isReversed() const;

	void setOrigins(QString const &motorPort, QString const &encoderPort);
	QString motorPort() const;
	QString encoderPort() const;

	float readEncoder();
	void resetEncoder();

	void setMotorPower(int power);
	void keepSpeed(float const metersPerSecond);

	void setIncrement(int const &increment);
	void increaseSpeed();
	void decreaseSpeed();

protected:
	trikControl::Motor *mMotor;
	trikControl::Encoder *mEncoder;
	int mPower;
	int mIncrement;
	int const mID;
	bool mIsReversed;
	bool mIsMotorBlocked;
	QString mMotorPort;
	QString mEncoderPort;
};

