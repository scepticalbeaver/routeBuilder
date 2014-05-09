#include <QtGui/QApplication>

#include "interactiveCommander.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	InteractiveCommander commander(app.thread(), InteractiveCommander::emulation);
	QTimer timer;
	int const startTime = 256;
	timer.singleShot(startTime, &commander, SLOT(start()));

	return app.exec();
}

