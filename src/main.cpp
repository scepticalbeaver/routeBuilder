#include <QtGui/QApplication>
#include <iostream>

#include "routeBuilder.h"

using namespace std;

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	RouteBuilder routeInspector(app.thread());

	QTimer timer;
	int const trackingTimeout = 30 * 1000; //msec

	while(true)
	{
		cout << endl << endl << "Type:\n"
			 << "1) tracking 30 sec to file\n"
			 << "2) write motor ports list\n"
			 << "3) read some sensor\n"
			 << "4) power on\n"
			 << "5) stop power\n"
			 << "9) continue exec\n"
			 << "0) exit program\n> ";
		int decision = 0;
		cin >> decision;
		cout << endl;

		switch (decision)
		{
		case 1:
			routeInspector.startTracking();
			timer.singleShot(trackingTimeout, &routeInspector, SLOT(stop()));
			return app.exec();
			break;
		case 2:
			foreach (QString const &line, routeInspector.motorList())
			{
				cout << line.toStdString() << " ";
			}
			cout << endl;

			break;
		case 3:
			cout << routeInspector.readSomeSensor();
			break;
		case 4:
			routeInspector.setPowerOnMotor(50);
			break;
		case 5:
			routeInspector.setPowerOnMotor(0);
		default:
			break;
		}

		if (decision == 9)
		{
			break;
		}

		if (decision == 0)
		{
			return 0;
		}

		if (timer.isActive())
			break;

	}

	return app.exec();
}

