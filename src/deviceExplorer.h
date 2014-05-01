#pragma once

#include <QtCore/QTimer>
#include <QtCore/QSettings>
#include <QtCore/QEventLoop>

#include <QDebug>

#include "motorComplect.h"

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

protected:
	trikControl::Brick mBrick;
	QVector<MotorComplect *> *mMotorsComplect;
	QSettings *mDeviceInfo;
	bool mHasSavedInfo;

	void loadDeviceConfiguration();
	void saveDevicesInfo();
	void saveDevice(MotorComplect const *motor);
	void sleep(unsigned int const &msec);
	void warmUpEngine(trikControl::Motor *motor);
	void resetEncoders();
	void resetEncoders(QList<trikControl::Encoder* > encoders);
	QMap<trikControl::Motor *, QString> motors();
	QMap<trikControl::Encoder *, QString> encoders();
};
