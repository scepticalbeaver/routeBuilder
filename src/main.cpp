#include <QtCore/QCoreApplication>

#include "routeBuilder.h"

using namespace std;

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	RouteBuilder routeInspector(app.thread());
	routeInspector.startTracking();

	int const trackingTimeout = 10 * 1000; //msec
	QTimer timer;
	timer.singleShot(trackingTimeout, &routeInspector, SLOT(stop()));

	return app.exec();
}

