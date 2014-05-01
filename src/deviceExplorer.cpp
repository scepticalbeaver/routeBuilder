#include "deviceExplorer.h"

using namespace trikControl;

namespace settingsKeys
{
enum Keys
{
	motorPort
	, encoderPort
	, isReversed
};
}

DeviceExplorer::DeviceExplorer(QThread *guiThread, QVector<MotorComplect *> *complects, QObject *parent)
	: QObject(parent)
	, mBrick(*guiThread, "./")
	, mMotorsComplect(complects)
	, mHasSavedInfo(false)
{
	QString settingsFile = "deviceInfo.ini";
	mDeviceInfo = new QSettings(settingsFile, QSettings::IniFormat, this);

	loadDeviceConfiguration();
}

DeviceExplorer::~DeviceExplorer()
{
	mDeviceInfo->sync();
}

void DeviceExplorer::reinitDevices()
{
	mMotorsComplect->clear();
	QMap<Motor*, QString> motors = motors();
	QMap<Encoder*, QString> encoders = encoders();
	float const epsilon = 10;
	foreach (Motor *motor, motors.keys())
	{
		resetEncoders(encoders.keys());
		warmUpEngine(motor);

		float max = 0;
		Encoder *machedEncoder = nullptr;
		foreach(Encoder *encoder, encoders.keys())
		{
			float absValue = qAbs(encoder->read());
			if (absValue < epsilon)
			{
				continue;
			}
			if (absValue > max)
			{
				max = absValue;
				machedEncoder = encoder;
			}
		}
		if (machedEncoder != nullptr)
		{
			qDebug() << "(motor, encoder) = " << motors.value(motor) << " " << encoders.value(machedEncoder);
			mMotorsComplect->append(new MotorComplect(motor, machedEncoder, mMotorsComplect->size()));
			mMotorsComplect->last()->setOrigins(motors.value(motor), encoders.value(machedEncoder));

			bool const isReversed = machedEncoder->read() < 0;
			qDebug() << "reversed?" << isReversed;
			mMotorsComplect->last()->setReversed(isReversed);
		}
	}
	qDebug() << "motors founded: " << mMotorsComplect->size();
	saveDevicesInfo();
}

void DeviceExplorer::loadDeviceConfiguration()
{
	mHasSavedInfo = mDeviceInfo->value("modified", false).toBool();
	if (!mHasSavedInfo)
	{
		return;
	}
	mMotorsComplect->clear();

	bool isDeviceConfigChanged = false;
	QString motorPort;
	QString encoderPort;
	foreach (QString groupId, mDeviceInfo->childGroups())
	{
		mDeviceInfo->beginGroup(groupId);
		motorPort = mDeviceInfo->value(settingsKeys::motorPort);
		encoderPort = mDeviceInfo->value(settingsKeys::encoderPort);
		bool const isReversed = mDeviceInfo->value(settingsKeys::isReversed);
		mDeviceInfo->endGroup();
		Motor *motor = mBrick.motor(motorPort);
		Encoder *encoder = mBrick.encoder(encoderPort);
		if (motor == nullptr || encoder == nullptr)
		{
			isDeviceConfigChanged = true;
			break;
		}
		mMotorsComplect->append(new MotorComplect(motor, encoder, groupId.toInt()));
		mMotorsComplect->last()->setReversed(isReversed);
		mMotorsComplect->last()->setOrigins(motorPort, encoderPort);
	}

	if (isDeviceConfigChanged)
	{
		qDebug() << "device configuration was changed. Make reinit!";
		mMotorsComplect->clear();
		return;
	}

	qDebug() << "Motor complects config was restored with " << mMotorsComplect->size() << " complects";
}

void DeviceExplorer::saveDevicesInfo()
{
	foreach (MotorComplect *complect, (*mMotorsComplect))
	{
		saveDevice(complect);
	}
	mDeviceInfo->setValue("modified", true);
}

void DeviceExplorer::saveDevice(MotorComplect const *complect)
{
	mDeviceInfo->beginGroup(complect->id());
	mDeviceInfo->setValue(settingsKeys::motorPort, complect->motorPort());
	mDeviceInfo->setValue(settingsKeys::encoderPort, complect->encoderPort());
	mDeviceInfo->setValue(settingsKeys::isReversed, complect->isReversed());
	mDeviceInfo->endGroup();
}

void DeviceExplorer::sleep(unsigned int const &msec)
{
	QEventLoop loop;
	QTimer::singleShot(msec, &loop, SLOT(quit()));
	loop.exec();
}

void DeviceExplorer::warmUpEngine(Motor *motor)
{
	int const testPower = 60;
	motor->setPower(testPower);
	sleep(500);
	motor->setPower(0);
}

void DeviceExplorer::resetEncoders()
{
	resetEncoders(encoders.keys());
}

void DeviceExplorer::resetEncoders(QList<Encoder *> encoders)
{
	foreach (Encoder *encoder, encoders)
	{
		encoder->reset();
	}
}

QMap<Motor *, QString> DeviceExplorer::motors()
{
	QMap<Motor *, QString> result;

	foreach(QString const &port, mBrick.motorPorts(Motor::powerMotor))
	{
		Motor *motor = mBrick.motor(port);
		if (motor != nullptr)
		{
			result.insert(motor, port);
		}
	}

	return result;
}

QMap<Encoder *, QString> DeviceExplorer::encoders()
{
	QMap<Encoder *, QString> result;
	QStringList encoders;
	encoders << "JB1" << "JB2" << "JB3" << "JB4" << "JM1" << "JM2" << "JM3" << "M1";

	foreach (QString const &ePort, encoders)
	{
		Encoder *encoder = mBrick.encoder(ePort);
		if (encoder != nullptr)
		{
			result.insert(encoder, ePort);
		}
	}
	return result;
}
