#include "robotWrap.h"

RobotWrapper::RobotWrapper(QThread *guiThread)
{
	mHasRealRobot = guiThread != nullptr;

	if (mHasRealRobot)
	{
		mBrick = new trikControl::Brick((*guiThread), "./");
		mBrickEmulator = nullptr;

		wrapRealDevices();
	}
	else
	{
		mBrickEmulator = new emulators::BrickEmulator();
		mBrick = nullptr;

		wrapEmulators();
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

void RobotWrapper::wrapRealDevices()
{
	QStringList encoders;
	encoders << "JB1" << "JB2" << "JB3" << "JB4" << "JM1" << "JM2" << "JM3" << "M1";

	foreach (QString const &encPort, encoders)
	{
		if (mBrick->encoder(encPort) != nullptr)
		{
			mEncoderWrappers.insert(encPort, new EncoderWrap(mBrick->encoder(encPort)));
		}
	}

	foreach (QString const &motorPort, mBrick->motorPorts(trikControl::Motor::powerMotor))
	{
		mMotorWrappers.insert(motorPort, new MotorWrap(mBrick->motor(motorPort)));
	}
}

void RobotWrapper::wrapEmulators()
{
	foreach (QString const &encPort, mBrickEmulator->encoderPorts())
	{
		mEncoderWrappers.insert(encPort, new EncoderWrap(mBrickEmulator->encoder(encPort)));
	}
	foreach (QString const &motorPort, mBrickEmulator->motorPorts())
	{
		mMotorWrappers.insert(motorPort, new MotorWrap(mBrickEmulator->motor(motorPort)));
	}
}

QStringList RobotWrapper::powerMotorPorts() const
{
	return mMotorWrappers.keys();
}

QStringList RobotWrapper::encoderPorts() const
{
	return mEncoderWrappers.keys();
}

MotorWrap *RobotWrapper::motor(QString const &port)
{
	return mMotorWrappers.value(port, nullptr);
}

EncoderWrap *RobotWrapper::encoder(QString const &port)
{
	return mEncoderWrappers.value(port, nullptr);
}
