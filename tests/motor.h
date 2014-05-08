#pragma once

#include <QObject>

namespace tests
{
class Motor : public QObject
{
	Q_OBJECT
public:
	explicit Motor(QObject *parent = 0);

	enum Type
	{
		powerMotor
		, servoMotor
	};
signals:

public slots:

};
}
