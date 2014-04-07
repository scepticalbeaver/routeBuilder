#include "interactiveCommander.h"

using std::cout;
using std::endl;

InteractiveCommander::InteractiveCommander(QThread &guiThread, QObject *parent)
	: QObject(parent)
	, mGuiThread(*guiThread)
{
	mRouteController = new RouteController(mGuiThread);
	mAlternativeThread = new QThread(this);
	mRouteController->moveToThread(mAlternativeThread);
	mAlternativeThread->start();
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
	connect(this, SIGNAL(turnMotorsRequested(bool)), mRouteController, SLOT(switchMotors(bool)));
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
		launchTracking();
		break;
	case 2:
		completeTracking();
		break;
	case 3:
		startPlayback();
		break;
	case 4:
		switchMotors(true);
		break;
	case 5:
		switchMotors(false);
		break;
	case 0:
		mGuiThread->terminate();
		break;
	}
}

void InteractiveCommander::printRoundMsg()
{
	cout << endl << endl << "Type:\n"
			<< "1) start tracking\n"
			<< "2) stop tracking\n"
			<< "3) repeat track\n"
			<< "4) turn on motors\n"
			<< "5) stop motors\n"
			<< "0) exit program\n> ";
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
	emit switchMotors(willTurnOn);
	cout << endl << "Sending message to motors..." << endl;
}

void InteractiveCommander::routeActionFinished(const bool successfully)
{
	cout << "Action was finished " << ((successfully)? "succesffuly\n" : "with fails\n");
	loopRound();
}
