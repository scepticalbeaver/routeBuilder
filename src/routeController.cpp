#include "routeController.h"

using namespace trikControl;

RouteController::RouteController(QThread *guiThread)
	: mGuiThread(guiThread)
	, mDeviceInfo(nullptr)
	, mStorage(nullptr)
	, mRouteRepeater(nullptr)
{
	qDebug() << "-- gui thread " << guiThread;
}

RouteController::~RouteController()
{
	if (mMotorsComplect == nullptr)
	{
		return;
	}
	qDeleteAll(*mMotorsComplect);
	delete mMotorsComplect;
}

void RouteController::afterThreadInit()
{
	mMotorsComplect = new QVector<MotorComplect *>;
	mDeviceInfo = new DeviceExplorer(mGuiThread, mMotorsComplect, this);
	mStorage = new TrackStorage(mMotorsComplect, this);
	mRouteRepeater = new RouteRepeater(mMotorsComplect, mStorage, this);
	connect(mRouteRepeater, SIGNAL(playbackDone()), SLOT(playbackStopped()));
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

void RouteController::startTracking()
{
	checkRAII();
	mStorage->startRecording();
	emit jobDone(true);
}

void RouteController::stopTracking()
{
	checkRAII();
	mStorage->stopRecording();
	emit jobDone(true);
}

void RouteController::playback()
{
	checkRAII();
	mRouteRepeater->playback();
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
	qDebug() << "has valid config: " << hasValidConfig;
	if (!hasValidConfig)
	{
		qDebug() << "re-init requested!";
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

