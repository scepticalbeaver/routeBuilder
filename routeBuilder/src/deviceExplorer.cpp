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
	, mBrickContainer(guiThread)
	, mMotorsComplect(complects)
	, mHasSavedInfo(false)
{
	QString settingsFile = "deviceInfo.ini";
	mDeviceInfo = new QSettings(settingsFile, QSettings::IniFormat, this);

	loadDeviceConfiguration();
	if (hasValidConfig())
	{
		emit devicesLoaded();
	}
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
	foreach (QString const &motorPort, mBrickContainer.powerMotorPorts())
	{
		MotorWrap *curMotor = mBrickContainer.motor(motorPort);
		resetEncoders();
		warmUpEngine(curMotor);

		float max = 0;
		QString matchedEncoder;
		foreach(QString const &encoderPort, mBrickContainer.encoderPorts())
		{
			float absValue = qAbs(mBrickContainer.encoder(encoderPort)->read());

			if (absValue < epsilon)
			{
				continue;
			}
			if (absValue > max)
			{
				max = absValue;
				matchedEncoder = encoderPort;
			}
		}

		if (!matchedEncoder.isEmpty())
		{
			qDebug() << "--(motor, encoder) = " << motorPort << " " << matchedEncoder;
			bool const isReversed = mBrickContainer.encoder(matchedEncoder)->read() < 0;
			qDebug() << "--reversed?" << isReversed;

			mMotorsComplect->append(new MotorComplect(
					mMotorsComplect->size()
					, curMotor
					, mBrickContainer.encoder(matchedEncoder)
					, isReversed
					));
			mMotorsComplect->last()->setOrigins(motorPort, matchedEncoder);
		}
	}
	qDebug() << "--motors founded: " << mMotorsComplect->size();
	saveDevicesInfo();
	if (mMotorsComplect->size())
	{
		emit devicesLoaded();
	}
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
	foreach (QString const &groupId, mDeviceInfo->childGroups())
	{
		mDeviceInfo->beginGroup(groupId);
		motorPort = mDeviceInfo->value(SettingsKeys::motorPort()).toString();
		encoderPort = mDeviceInfo->value(SettingsKeys::encoderPort()).toString();
		bool const isReversed = mDeviceInfo->value(SettingsKeys::isReversed()).toBool();
		mDeviceInfo->endGroup();
		MotorWrap *motor = mBrickContainer.motor(motorPort);
		EncoderWrap *encoder = mBrickContainer.encoder(encoderPort);
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
		qDebug() << "--device configuration was changed. Make reinit!";
		qDeleteAll(*mMotorsComplect);
		mMotorsComplect->clear();
		mHasSavedInfo = false;
		return;
	}

	qDebug() << "--Motor complects config was restored with " << mMotorsComplect->size() << " complects";
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

void DeviceExplorer::warmUpEngine(MotorWrap *motor)
{
	int testPower = 60;
	motor->setPower(testPower);
	sleep(500);
	motor->setPower(0);
}

void DeviceExplorer::resetEncoders()
{
	foreach (QString const &encPort, mBrickContainer.encoderPorts())
	{
		mBrickContainer.encoder(encPort)->reset();
	}
}
