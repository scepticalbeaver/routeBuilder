#include <QtCore/QCoreApplication>

#include "routeBuilder.h"

using namespace std;

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	RouteBuilder routeInspector(app.thread());

	Q_UNUSED(routeInspector);
	return app.exec();
}

