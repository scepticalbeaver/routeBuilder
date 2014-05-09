#pragma once

#include <QtCore/QThread>

#include "trikControl/brick.h"
#include "../../emulatorTest/brickEmulator.h"

#include "motorWrap.h"
#include "encoderWrap.h"

class RobotWrapper : public QObject
{
	Q_OBJECT
public:
	explicit RobotWrapper(QThread *guiThread);
	~RobotWrapper();


public slots:
	/// Retruns list of ports for motors of a given type.
	QStringList motorPorts(trikControl::Motor::Type type) const;

	/// Retruns list of ports for encoders
	QStringList encoderPorts() const;


	/// Returns reference to motor of a given type on a given port
	MotorWrap *motor(QString const &port);

	/// Returns reference to encoder on given port.
	EncoderWrap *encoder(QString const &port);

protected:
	bool mHasRealRobot;
	trikControl::Brick *mBrick;
	emulators::BrickEmulator *mBrickEmulator;

	QHash<QString, MotorWrap *> mRealMotorWrappers;
	QHash<QString, EncoderWrap *> mRealEncoderWrappers;
};

