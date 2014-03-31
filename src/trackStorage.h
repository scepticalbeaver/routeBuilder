#pragma once

#include <QtCore/QVector>
#include <QtCore/QPair>
#include <QtCore/QFile>

#include <QDebug>

class TrackStorage
{
public:
	TrackStorage();
	~TrackStorage();

	void addValue(float const &value);
	void printToFile();

protected:
	int const epsilon = 20;

	QVector<QPair<float,int> > mSensor1;
};

