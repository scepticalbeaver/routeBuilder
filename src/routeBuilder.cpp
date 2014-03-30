#include "routeBuilder.h"

RouteBuilder::RouteBuilder(QThread *mainThread)
	: brick(*mainThread, QString(""))
{
}
