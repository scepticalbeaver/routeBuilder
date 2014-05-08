#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QThread>
#include <QtCore/QHash>
#include <QtCore/QStringList>

#include "motor.h"
#include "encoder.h"

namespace tests
{
class BrickEmulator : public QObject
{
	Q_OBJECT
public:
	BrickEmulator();

	~BrickEmulator();

public slots:
	/// Stops all motors and shuts down all current activity.
	void stop();

	/// Returns reference to motor of a given type on a given port
	Motor *motor(QString const &port);

	/// Retruns list of ports for motors of a given type.
	QStringList motorPorts(Motor::Type type) const;

	/// Returns list of PWM signal capture device ports.
	QStringList pwmCapturePorts() const;

	/// Returns reference to encoder on given port.
	Encoder *encoder(QString const &port);

private:
	QHash<QString, Motor *> mPowerMotors;  // Has ownership.
	QHash<QString, Encoder *> mEncoders;  // Has ownership.
};

}

