#include "coordinateMapper.h"

#include <math.h>
#include "constants.h"

using namespace robotConstants;

CoordinateMapper::CoordinateMapper(QVector<MotorComplect *> *complects, const int &leftMotorId, const int &rightMotorId
			, QObject *parent)
	: QObject(parent)
	, mPosition(0, 0)
	, mThetta(0)
	, mMotorComplects(complects)
	, mLeftMotor(nullptr)
	, mRightMotor(nullptr)
	, mLeftMotorId(leftMotorId)
	, mRightMotorId(rightMotorId)
	, mTimeStamp(0)
{
	connect(&mTimer, SIGNAL(timeout()), SLOT(update()));
}

QPointF CoordinateMapper::pos() const
{
	return mPosition;
}

float CoordinateMapper::angle() const
{
	return mThetta;
}

float CoordinateMapper::angleDegrees() const
{
	return static_cast<int>(std::nearbyint(180 * mThetta / RobotSpecifications::pi())) % 360;
}

void CoordinateMapper::setTrackingMotors(int const &leftMotorId, int const &rightMotorId)
{
	foreach(MotorComplect *motor, (*mMotorComplects))
	{
		if (motor->id() == leftMotorId)
		{
			mLeftMotor = motor;
		}
		if (motor->id() == rightMotorId)
		{
			mRightMotor = motor;
		}
	}
}

void CoordinateMapper::reset()
{
	mTimer.stop();
	mPrevValue.clear();
	mPosition = QPointF(0, 0);
	mThetta = 0;
	setTrackingMotors(mLeftMotorId, mRightMotorId);
}

void CoordinateMapper::setupCoordinateSystem(QPointF const &currentPoint, float const &angleRad)
{
	bool isTimerActive = mTimer.isActive();
	mTimer.stop();

	mPrevValue.insert(mLeftMotor, mLeftMotor->readEncoder());
	mPrevValue.insert(mRightMotor, mRightMotor->readEncoder());
	mPosition = currentPoint;
	mThetta = angleRad;

	if (isTimerActive)
	{
		mTimer.start(timeout);
	}
}

void CoordinateMapper::start()
{
	mTimer.start(timeout);
}

void CoordinateMapper::stop()
{
	mTimer.stop();
}

QPointF CoordinateMapper::computeOdometryOffset(float const &leftDistance, float const &rightDistance)
{
	if (qFuzzyCompare(leftDistance, rightDistance))
	{
		return QPointF(leftDistance * std::cos(mThetta), leftDistance * std::sin(mThetta));
	}

	qDebug() << "\nLLen:" << leftDistance << "\tRLen" << rightDistance;

	// {R2 - R1 = width,  R1 / R2 = S1 / S2} , k * 2*pi*R = S
	float const leftRadius = RobotSpecifications::wheelBase * leftDistance / (rightDistance - leftDistance);
	float const rightRadius = leftRadius + RobotSpecifications::wheelBase;
	float const averageDistance = (leftDistance + rightDistance) / 2;

	//calc rotation coefficent "k"
	float rotationCoeff = 0;
	if (qFuzzyIsNull(leftRadius))
	{
		rotationCoeff = rotationCoeffCompute(rightDistance, rightRadius);
	}
	else
	{
		float const rotationCoeffLeft = rotationCoeffCompute(leftDistance, leftRadius);
		if (!qFuzzyIsNull(rightRadius))
		{
			// average
			rotationCoeff = (rotationCoeffLeft + rotationCoeffCompute(rightDistance, rightRadius)) / 2;
		}
	}

	// delta Thetta
	float const dTh = 2 * RobotSpecifications::pi() * rotationCoeff;
	mThetta += dTh;

	return QPointF(averageDistance * std::cos(mThetta), averageDistance * std::sin(mThetta));
}

float CoordinateMapper::rotationCoeffCompute(float const &distance, float const &radius)
{
	return distance / (2 * RobotSpecifications::pi() * radius);
}

void CoordinateMapper::update()
{
	float const encLeft = mLeftMotor->readEncoder();
	float const encRight = mRightMotor->readEncoder();
	float const leftDifference = encLeft - mPrevValue.value(mLeftMotor, encLeft);
	float const rightDifference = encRight - mPrevValue.value(mRightMotor, encRight);
	mPrevValue.insert(mLeftMotor, encLeft);
	mPrevValue.insert(mRightMotor, encRight);
	float const leftDistance = RobotSpecifications::pi() * leftDifference / RobotSpecifications::encodersPerRound;
	float const rightDistance = RobotSpecifications::pi() * rightDifference / RobotSpecifications::encodersPerRound;

	if (qFuzzyIsNull(qAbs(leftDifference) + qAbs(rightDifference)))
	{
		return;
	}
	mPosition += computeOdometryOffset(leftDistance, rightDistance);

	mTimeStamp = (mTimeStamp + 1) % 5;
	if (!mTimeStamp)
	{
		qDebug() << "--Pos:\t" << mPosition << "\tangle:\t" << angleDegrees();
	}
}
