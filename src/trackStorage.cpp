#include "trackStorage.h"

TrackStorage::TrackStorage()
{
}

TrackStorage::~TrackStorage()
{
	mSensor1.clear();
}

void TrackStorage::addValue(int const &value)
{
	if (mSensor1.count() == 0 || qAbs(mSensor1.last().first - value) > epsilon)
	{
		mSensor1.append(qMakePair(value, 0));
		return;
	}

	mSensor1.last() = qMakePair(mSensor1.first(), mSensor1.last() + 1);

}

void TrackStorage::printToFile()
{
	QFile output("routeBuilder_filteredData.txt");
	if (!output.open(QFile::WriteOnly))
	{
		qDebug() << "Problem with writting filtered data to file";
		return;
	}

	QString line("");
	foreach (QPair<int,int> const &pair, mSensor1)
	{
		line = QString::number(pair.first) + "\t\t" + QString::number(pair.second) + "\n";
		output.write(line.toUtf8());
	}

	output.close();
}
