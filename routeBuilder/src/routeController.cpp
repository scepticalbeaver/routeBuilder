#include "routeController.h"
#include "constants.h"

using namespace trikControl;
using namespace keywords;

RouteController::RouteController(QThread *guiThread)
	: mGuiThread(guiThread)
	, mDeviceInfo(nullptr)
	, mStorage(nullptr)
	, mRouteRepeater(nullptr)
	, mMotorsComplect(nullptr)
	, mCoordinateMapper(nullptr)
{
}

RouteController::~RouteController()
{
	mCoordinateMapper->stop();

	if (mMotorsComplect == nullptr)
	{
		return;
	}

	foreach (MotorComplect *complect, (*mMotorsComplect))
	{
		complect->setMotorPower(0);
		delete complect;
	}

	delete mMotorsComplect;
}

void RouteController::afterThreadInit()
{
	mMotorsComplect = new QVector<MotorComplect *>;
	mDeviceInfo = new DeviceExplorer(mGuiThread, mMotorsComplect, this);
	mStorage = new TrackStorage(mMotorsComplect, this);
	mRouteRepeater = new RouteRepeater(mMotorsComplect, mStorage, this);
	mCoordinateMapper = new CoordinateMapper(mMotorsComplect, 0, 1, this);

	connect(mRouteRepeater, SIGNAL(playbackDone()), SLOT(playbackStopped()));
	connect(mDeviceInfo, SIGNAL(devicesLoaded()), mCoordinateMapper, SLOT(reset()));

	if (mDeviceInfo->hasValidConfig())
	{
		mCoordinateMapper->reset();
		mCoordinateMapper->start();
	}
}


void RouteController::playbackStopped()
{
	emit jobDone(true);
}

void RouteController::turnPowerMotors(int const power)
{
	foreach(MotorComplect *complect, (*mMotorsComplect))
	{
		complect->setMotorPower(power);
	}
}

void RouteController::checkRAII()
{
	if (mMotorsComplect != nullptr)
	{
		return;
	}

	afterThreadInit();
}

void RouteController::resetEncoders()
{
	foreach (MotorComplect *motor, (*mMotorsComplect))
	{
		motor->resetEncoder();
	}
}

void RouteController::startTracking()
{
	checkRAII();
	resetEncoders();
	mStorage->startRecording(TrackingFlows::mainTrackingFlow);
	emit jobDone(true);
}

void RouteController::stopTracking()
{
	checkRAII();
	mStorage->stopRecording();
	emit jobDone(true);
}

void RouteController::loadTrackFromFile()
{
	checkRAII();
	emit jobDone(mStorage->loadFromFile(QString("data.log"), TrackingFlows::mainTrackingFlow));
}

void RouteController::playback()
{
	checkRAII();
	resetEncoders();
	mRouteRepeater->playback(TrackingFlows::mainTrackingFlow);
}

void RouteController::switchMotors(bool const willTurnOn)
{
	checkRAII();
	int const power = (willTurnOn)? 85 : 0;
	turnPowerMotors(power);
	emit jobDone(true);
}

void RouteController::initDevices()
{
	checkRAII();
	mDeviceInfo->reinitDevices();
	emit jobDone(mMotorsComplect->size() > 0);
}

void RouteController::checkLoadedDevices()
{
	checkRAII();
	bool const hasValidConfig = mDeviceInfo->hasValidConfig();
	qDebug() << "--has valid config: " << hasValidConfig;
	if (!hasValidConfig)
	{
		qDebug() << "--re-init requested!";
		emit jobDone(false);
		return;
	}

	qDebug() << "Complects: " << mMotorsComplect->size();
	foreach (MotorComplect *complect, (*mMotorsComplect))
	{
		qDebug() << "id: " << complect->id()
				<< "  motor: " << complect->motorPort()
				<< "  encoder: " << complect->encoderPort()
				<< "  isReversed: " << complect->isReversed() << "\n";
	}
	emit jobDone(true);
}

