#pragma once

#include <QDebug>

#include "trikControl/brick.h"

//! @class MotorComplect is a conformity of Motor and Encoder
class MotorComplect : public QObject
{
	Q_OBJECT
public:
	//! @arg complectID must be unique identificator of complect
	MotorComplect(int const &complectID, trikControl::Motor *motor, trikControl::Encoder *motorEncoder
			, bool const &isReversed);

	//! @return unique identificator of motor complect
	int id() const;

	//! @return is forward direction reversed by encoders indication
	bool isReversed() const;

	//! sets port names for serializing and saving
	void setOrigins(QString const &motorPort, QString const &encoderPort);
	QString motorPort() const;
	QString encoderPort() const;

	float readEncoder();
	void resetEncoder();

	void setMotorPower(int power);
	void keepSpeed(float const &metersPerSecond);

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

