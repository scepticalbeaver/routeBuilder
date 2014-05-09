#pragma once

#include <QtCore/QString>
#include <QtCore/QThread>
#include <QtCore/QHash>
#include <QtCore/QStringList>

#include "../routeBuilder/src/wrappers/motorWrap.h"
#include "../routeBuilder/src/wrappers/encoderWrap.h"

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
	QStringList motorPorts(trikControl::Motor::Type type) const;

	/// Retruns list of ports for encoder
	QStringList encoderPorts() const;

	/// Returns reference to motor of a given type on a given port
	MotorWrap *motor(QString const &port);

	/// Returns reference to encoder on given port.
	EncoderWrap *encoder(QString const &port);

private:
	int const mMotorDevicesCount;
	QHash<QString, MotorWrap *> mPowerMotors;
	QHash<QString, EncoderWrap *> mEncoders;
	QList<MotorDevice *> mMotorDevices;
};

}

