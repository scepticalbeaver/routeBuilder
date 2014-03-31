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

	mSensor1.last() = qMakePair(mSensor1.last().first, mSensor1.last().second + 1);

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
	QPair<int, int> current;
	for(int i = 0; i < mSensor1.count(); i++)
	{
		current = mSensor1.at(i);
		line = QString::number(current.first) + "\t\t" + QString::number(current.second) + "\n";
		output.write(line.toUtf8());
	}

	output.close();
}
