#pragma once

#include <QtCore/QTimer>
#include <QtCore/QSettings>
#include <QtCore/QEventLoop>

#include <QDebug>

#include "motorComplect.h"
#include "wrappers/robotWrap.h"

//! @class DeviceExplorer stores in QSettings all MotorComplects
//! provide initialization, saving and restoring device configuration
class DeviceExplorer : public QObject
{
	Q_OBJECT
public:
	//! @arg complects constructor loads saved config to vector
	DeviceExplorer(QThread *guiThread, QVector<MotorComplect *> *complects, QObject *parent = 0);

	~DeviceExplorer();

	//! explore device to make conformity of motors and encoders
	//! updates motor-complects vector and saves settings to file
	void reinitDevices();

	bool hasValidConfig() const;
signals:
	void devicesLoaded();

protected:
	RobotWrapper mBrickContainer;
	QVector<MotorComplect *> *mMotorsComplect;
	QSettings *mDeviceInfo;
	bool mHasSavedInfo;

	void loadDeviceConfiguration();
	void saveDevicesInfo();
	void saveDevice(MotorComplect const *complect);
	void sleep(unsigned int const &msec);
	void warmUpEngine(MotorWrap *motor);
	void resetEncoders();

	QMap<trikControl::Encoder *, QString> encoders();
};
