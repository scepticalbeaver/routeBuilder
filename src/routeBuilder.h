#pragma once

#include <QtCore/QObject>
#include <QtCore/QTimer>

#include "trikControl/brick.h"

using namespace trikControl;

class RouteBuilder : public QObject
{
	Q_OBJECT
public:
	explicit RouteBuilder(QThread *mainThread);

protected:
	Brick brick;


};
