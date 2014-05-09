#pragma once

#include <QtCore/QThread>
#include <QtCore/QHash>
#include <QtCore/QStringList>

#include "motorEmulator.h"
#include "encoderEmulator.h"

namespace emulators
{
class BrickEmulator : public QObject
{
	Q_OBJECT
public:
	BrickEmulator(int const &motorsCount = 2);

	~BrickEmulator() {}

public slots:
	/// Retruns list of ports for motors of a given type.
	QStringList motorPorts() const;

	/// Retruns list of ports for encoder
	QStringList encoderPorts() const;

	/// Returns reference to motor of a given type on a given port
	MotorEmulator *motor(QString const &port);

	/// Returns reference to encoder on given port.
	EncoderEmulator *encoder(QString const &port);

private:
	int const mMotorDevicesCount;
	QHash<QString, MotorEmulator *> mPowerMotors;
	QHash<QString, EncoderEmulator *> mEncoders;
	QList<MotorDevice *> mMotorDevices;
};

}

