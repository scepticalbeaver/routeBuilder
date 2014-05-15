#pragma once

#include <QtCore/QTimer>
#include <QtCore/QPointF>

#include "motorComplect.h"

//! @class CoordinateMapper transforms encoder information into local metric coordinate system
class CoordinateMapper : public QObject
{
	Q_OBJECT
public:
	//! Constructor
	//! @param complects array of powermotors

	//! @param parent for Qt objects hierarchy
	CoordinateMapper(QVector<MotorComplect *> *complects, int const &leftMotorId, int const &rightMotorId
			, QObject *parent = 0);

public slots:
	//! @return robot position (meters)
	QPointF pos() const;
	//! @return robot angle in radians
	float angle() const;
	//! @return robot angle in degrees
	float angleDegrees() const;

	//! mark motors as left and right to make orientation
	void setTrackingMotors(int const &leftMotorId, int const &rightMotorId);

	//! Reset coordinate system to zero
	void reset();

	//! Correct coordinate system
	void setupCoordinateSystem(QPointF const &currentPoint, float const &angleRad = 0);

	void start();
	void stop();

protected:
	static int const timeout = 200;
	QTimer mTimer;
	QMap<MotorComplect*, float> mPrevValue;
	QPointF mPosition;
	// Rotation in coordinate system
	float mThetta;
	QVector<MotorComplect *> *mMotorComplects;
	MotorComplect *mLeftMotor;
	MotorComplect *mRightMotor;
	int mLeftMotorId;
	int mRightMotorId;
	int mTimeStamp;

	QPointF computeOdometryOffset(float const &leftDistance, float const &rightDistance);

	float rotationCoeffCompute(float const &distance, float const &radius);

protected slots:
	void update();
};
