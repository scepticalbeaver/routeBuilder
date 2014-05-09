#include "interactiveCommander.h"

using std::cin;
using std::cout;
using std::endl;

InteractiveCommander::InteractiveCommander(QThread *guiThread, RobotType robotType)
{
	qDebug() << "-- gui thread:\t" << guiThread;
	mRouteController = new RouteController((robotType == realConnection)? guiThread : nullptr);
	mAlternativeThread = new QThread(this);
	mRouteController->moveToThread(mAlternativeThread);
	mAlternativeThread->start();
	initConnections();
}

InteractiveCommander::~InteractiveCommander()
{
	stopAuxThread();
	delete mRouteController;
	delete mAlternativeThread;
}

void InteractiveCommander::start()
{
	loopRound();
}

void InteractiveCommander::initConnections()
{
	connect(this, SIGNAL(trackingRequested()), mRouteController, SLOT(startTracking()), Qt::QueuedConnection);
	connect(this, SIGNAL(stopTrackingRequested()), mRouteController, SLOT(stopTracking()), Qt::QueuedConnection);
	connect(this, SIGNAL(turnMotorsRequested(bool)), mRouteController, SLOT(switchMotors(bool)), Qt::QueuedConnection);
	connect(this, SIGNAL(initDevicesRequest()), mRouteController, SLOT(initDevices()), Qt::QueuedConnection);
	connect(this, SIGNAL(playbackRequested()), mRouteController, SLOT(playback()), Qt::QueuedConnection);
	connect(this, SIGNAL(checkingDevice()), mRouteController, SLOT(checkLoadedDevices()), Qt::QueuedConnection);
	connect(mRouteController, SIGNAL(jobDone(bool)), SLOT(routeActionFinished(bool)), Qt::QueuedConnection);
}

void InteractiveCommander::loopRound()
{
	printRoundMsg();
	int decision = 0;
	cin >> decision;
	cout << endl;

	switch (decision)
	{
	case 1:
		emit checkingDevice();
		break;
	case 2:
		initDevicesSignal();
		break;
	case 3:
		launchTracking();
		break;
	case 4:
		completeTracking();
		break;
	case 5:
		startPlayback();
		break;
	case 6:
		switchMotors(true);
		break;
	case 7:
		switchMotors(false);
		break;
	case 0:
		stopAuxThread();
		QApplication::quit();
		break;
	}
}

void InteractiveCommander::printRoundMsg()
{
	cout << endl << endl << "Type:\n"
			<< "1) check loaded devices\n"
			<< "2) init devices\n"
			<< "3) start tracking\n"
			<< "4) stop tracking\n"
			<< "5) repeat track\n"
			<< "6) turn on motors\n"
			<< "7) stop motors\n"
			<< "0) exit program\n> ";
}

void InteractiveCommander::initDevicesSignal()
{
	emit initDevicesRequest();
	cout << endl << "Devices initialization..." << endl;
}

void InteractiveCommander::launchTracking()
{
	emit trackingRequested();
	cout << endl << "Start tracking..." << endl;
}

void InteractiveCommander::completeTracking()
{
	emit stopTrackingRequested();
	cout << endl << "Tracking done" << endl;
}

void InteractiveCommander::startPlayback()
{
	emit playbackRequested();
	cout << endl << "Repeating route..." << endl;
}

void InteractiveCommander::switchMotors(bool const willTurnOn)
{
	emit turnMotorsRequested(willTurnOn);
	cout << endl << "Sending message to motors..." << endl;
}

void InteractiveCommander::stopAuxThread()
{
	int const timeoutMSec = 500;
	if (mAlternativeThread->isRunning())
	{
		mAlternativeThread->quit();
		mAlternativeThread->wait(timeoutMSec);
	}
}

void InteractiveCommander::routeActionFinished(const bool successfully)
{
	cout << "Action was finished " << ((successfully)? "succesffuly\n" : "with fails\n");
	loopRound();
}
