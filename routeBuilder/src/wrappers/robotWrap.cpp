#include "robotWrap.h"

RobotWrapper::RobotWrapper(QThread *guiThread)
{
	mHasRealRobot = guiThread != nullptr;

	if (mHasRealRobot)
	{
		mBrick = new trikControl::Brick((*guiThread), "./");
		mBrickEmulator = nullptr;

		QStringList encoders;
		encoders << "JB1" << "JB2" << "JB3" << "JB4" << "JM1" << "JM2" << "JM3" << "M1";

		foreach (QString const &encPort, encoders)
		{
			if (mBrick->encoder(encPort) != nullptr)
			{
				mRealEncoderWrappers.insert(encPort, new EncoderWrap(mBrick->encoder(encPort)));
			}
		}

		foreach (QString const &motorPort, mBrick->motorPorts(trikControl::Motor::powerMotor))
		{
			mRealMotorWrappers.insert(motorPort, new MotorWrap(mBrick->motor(motorPort)));
		}
	}
	else
	{
		mBrickEmulator = new emulators::BrickEmulator();
		mBrick = nullptr;
	}
}

RobotWrapper::~RobotWrapper()
{
	if (mHasRealRobot)
	{
		delete mBrick;
	}
	else
	{
		delete mBrickEmulator;
	}
}

QStringList RobotWrapper::motorPorts(trikControl::Motor::Type type) const
{
	if (mHasRealRobot)
	{
		return mRealMotorWrappers.keys();
	}
	else
	{
		return mBrickEmulator->motorPorts(type);
	}
}

QStringList RobotWrapper::encoderPorts() const
{
	if (mHasRealRobot)
	{
		return mRealEncoderWrappers.keys();
	}
	else
	{
		return mBrickEmulator->encoderPorts();
	}
}

MotorWrap *RobotWrapper::motor(QString const &port)
{
	if (mHasRealRobot)
	{
		return mRealMotorWrappers.value(port, nullptr);
	}
	else
	{
		return mBrickEmulator->motor(port);
	}
}

EncoderWrap *RobotWrapper::encoder(QString const &port)
{
	if (mHasRealRobot)
	{
		return mRealEncoderWrappers.value(port, nullptr);
	}
	else
	{
		return mBrickEmulator->encoder(port);
	}
}
