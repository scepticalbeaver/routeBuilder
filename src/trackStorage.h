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

	void addValue(int const &value);
	void printToFile();

protected:
	int const epsilon = 10;

	QVector<QPair<int,int> > mSensor1;
};

