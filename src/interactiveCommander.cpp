#include "interactiveCommander.h"

using std::cin;
using std::cout;
using std::endl;

InteractiveCommander::InteractiveCommander(QThread *guiThread, QObject *parent)
	: QObject(parent)
	, mGuiThread(guiThread)
{
	mRouteController = new RouteController(mGuiThread);
	mAlternativeThread = new QThread(this);
	mRouteController->moveToThread(mAlternativeThread);
	mAlternativeThread->start();
	initConnections();
}

InteractiveCommander::~InteractiveCommander()
{
	int const timeout = 1000;
	mAlternativeThread->terminate();
	mAlternativeThread->wait(timeout);
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
	connect(mRouteController, SIGNAL(jobDone(bool)), SLOT(routeActionFinished(bool)));
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
		initDevicesSignal();
		break;
	case 2:
		launchTracking();
		break;
	case 3:
		completeTracking();
		break;
	case 4:
		startPlayback();
		break;
	case 5:
		switchMotors(true);
		break;
	case 6:
		switchMotors(false);
		break;
	case 0:
		mAlternativeThread->quit();
		mGuiThread->quit();
		break;
	}
}

void InteractiveCommander::printRoundMsg()
{
	cout << endl << endl << "Type:\n"
			<< "1) init devices (do it first!)\n"
			<< "2) start tracking\n"
			<< "3) stop tracking\n"
			<< "4) repeat track\n"
			<< "5) turn on motors\n"
			<< "6) stop motors\n"
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

void InteractiveCommander::routeActionFinished(const bool successfully)
{
	cout << "Action was finished " << ((successfully)? "succesffuly\n" : "with fails\n");
	loopRound();
}
