#pragma once

#include <QtCore/QTimer>
#include <math.h>

#include <QDebug>

namespace emulators
{
class MotorDevice : public QObject
{
	Q_OBJECT
public:
	explicit MotorDevice(QObject *parent = 0);
	~MotorDevice();

public slots:
	void resetEncoder();

	float readEncoder() const;

	void setPower(int const &value);
	int power() const;

protected:
	static int const timeout = 10;
	static constexpr double accelerate = 1.64; //mpss
	static constexpr double maxSpeed = 1.8; // mps
	static constexpr float encPerRound = 420;
	static constexpr double diameter = 0.2; //meters
	static constexpr double pi()
	{
		return 4 * std::atan(1);
	}

	static constexpr double roundLength()
	{
		return pi() * diameter; //meters
	}

	int mPower;
	float mEncoderValue;
	double mDestSpeed;
	double mSpeed;

	QTimer mUpdater;

	void adjustSpeed();

protected slots:
	void updateState();
};
}

