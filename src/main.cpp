#include <QtGui/QApplication>
#include <iostream>

#include "routeBuilder.h"

using namespace std;

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	RouteBuilder routeInspector(app.thread());

	QTimer timer;
	int const trackingTimeout = 10 * 1000; //msec

	while(true)
	{
		cout << endl << endl << "Type:\n"
			 << "1) tracking 10 sec to file\n"
			 << "2) write motor ports list\n"
			 << "3) write digital ports list\n"
			 << "4) read some sensor\n"
			 << "0) exit\n> ";
		int decision = 0;
		cin >> decision;
		cout << endl;

		switch (decision)
		{
		case 1:
			routeInspector.startTracking();
			timer.singleShot(trackingTimeout, &routeInspector, SLOT(stop()));
			break;
		case 2:
			foreach (QString const &line, routeInspector.motorList())
			{
				cout << line.toStdString() << " ";
			}
			cout << endl;

			break;
		case 3:
			foreach (QString const &line, routeInspector.digitalList())
			{
				cout << line.toStdString() << " ";
			}
			cout << endl;
			break;
		case 4:
			cout << routeInspector.readSomeSensor();
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

