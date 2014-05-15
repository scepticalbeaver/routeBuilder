#pragma once

#include <QtCore/QTimer>

#include <QDebug>

#include "wrappers/motorWrap.h"
#include "wrappers/encoderWrap.h"

//! @class MotorComplect is a conformity of Motor and Encoder
class MotorComplect : public QObject
{
	Q_OBJECT
public:
	//! @arg complectID must be unique identificator of complect
	MotorComplect(int const &complectID, MotorWrap *motor, EncoderWrap *motorEncoder
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
	void setSpeed(float const &metersPerSecond);

	void setIncrement(int const &increment);
	void increaseSpeed();
	void decreaseSpeed();

protected:
	static int const timeout = 20;

	MotorWrap *mMotor;
	EncoderWrap *mEncoder;
	int mPower;
	int mIncrement;
	int const mID;
	bool mIsReversed;
	bool mIsMotorBlocked;
	float mPrevEncoderDiff;
	float mPrevEncoderValue;
	float mProperEncDiff;
	QString mMotorPort;
	QString mEncoderPort;
	QTimer mSpeedKeeper;

	bool isStable(float const &curEncoder) const;

protected slots:
	void keepSpeed();
};

