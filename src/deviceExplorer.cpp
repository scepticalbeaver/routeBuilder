#include "deviceExplorer.h"

using namespace trikControl;

class SettingsKeys
{
public:
	static QString motorPort()
	{
		return QString("motorPort");
	}
	static QString encoderPort()
	{
		return QString("encoderPort");
	}
	static QString isReversed()
	{
		return QString("isReversed");
	}
};

DeviceExplorer::DeviceExplorer(QThread *guiThread, QVector<MotorComplect *> *complects, QObject *parent)
	: QObject(parent)
	, mBrick(*guiThread, "./")
	, mMotorsComplect(complects)
	, mHasSavedInfo(false)
{
	QString settingsFile = "deviceInfo.ini";
	mDeviceInfo = new QSettings(settingsFile, QSettings::IniFormat, this);

	//loadDeviceConfiguration();
}

DeviceExplorer::~DeviceExplorer()
{
	mDeviceInfo->sync();
}

void DeviceExplorer::reinitDevices()
{
	qDeleteAll(*mMotorsComplect);
	mMotorsComplect->clear();

	float const epsilon = 5;
	foreach (QString const &motorPort, mBrick.motorPorts(Motor::powerMotor))
	{
		Motor *curMotor = mBrick.motor(motorPort);
		resetEncoders();
		warmUpEngine(curMotor);

		float max = 0;
		Encoder *machedEncoder = nullptr;
		foreach(Encoder *encoder, encoders().keys())
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
			qDebug() << "(motor, encoder) = " << motorPort << " " << encoders().value(machedEncoder);
			bool const isReversed = machedEncoder->read() < 0;
			qDebug() << "reversed?" << isReversed;

			mMotorsComplect->append(new MotorComplect(
					mMotorsComplect->size()
					, curMotor
					, machedEncoder
					, isReversed
					));
			mMotorsComplect->last()->setOrigins(motorPort, encoders().value(machedEncoder));
		}
	}
	qDebug() << "motors founded: " << mMotorsComplect->size();
	saveDevicesInfo();
}

bool DeviceExplorer::hasValidConfig() const
{
	return mHasSavedInfo;
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
		motorPort = mDeviceInfo->value(SettingsKeys::motorPort()).toString();
		encoderPort = mDeviceInfo->value(SettingsKeys::encoderPort()).toString();
		bool const isReversed = mDeviceInfo->value(SettingsKeys::isReversed()).toBool();
		mDeviceInfo->endGroup();
		Motor *motor = mBrick.motor(motorPort);
		Encoder *encoder = mBrick.encoder(encoderPort);
		if (motor == nullptr || encoder == nullptr)
		{
			isDeviceConfigChanged = true;
			break;
		}
		mMotorsComplect->append(new MotorComplect(groupId.toInt(), motor, encoder, isReversed));
		mMotorsComplect->last()->setOrigins(motorPort, encoderPort);
	}

	if (isDeviceConfigChanged)
	{
		qDebug() << "device configuration was changed. Make reinit!";
		qDeleteAll(*mMotorsComplect);
		mMotorsComplect->clear();
		mHasSavedInfo = false;
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
	mDeviceInfo->beginGroup(QString::number(complect->id()));
	mDeviceInfo->setValue(SettingsKeys::motorPort(), complect->motorPort());
	mDeviceInfo->setValue(SettingsKeys::encoderPort(), complect->encoderPort());
	mDeviceInfo->setValue(SettingsKeys::isReversed(), complect->isReversed());
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
	foreach (Encoder *encoder, encoders().keys())
	{
		encoder->reset();
	}
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
