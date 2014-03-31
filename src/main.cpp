#include <QtGui/QApplication>
#include <iostream>

#include "routeBuilder.h"

using namespace std;

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	RouteBuilder routeInspector(app.thread());

	QTimer timer;

	while(true)
	{
		cout << endl<< "Type:\n"
			 << "1) tracking 10 sec to file\n"
			 << "2) write motor ports list\n"
			 << "3) write digital ports list\n"
			 << "4) read some sensor\n"
			 << "0) exit";
		int decision = 0;
		cin >> decision;

		switch (decision)
		{
		case 1:
			routeInspector.startTracking();

			int const trackingTimeout = 10 * 1000; //msec
			timer.singleShot(trackingTimeout, &routeInspector, SLOT(stop()));
			break;
		case 2:
			routeInspector.motorList();
			break;
		case 3:
			routeInspector.digitalList();
			break;
		case 4:
			routeInspector.readSomeSensor();
			break;
		default:
			break;
		}

		if (decision == 0)
		{
			break;
		}

	}

	return app.exec();
}

