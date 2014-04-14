#include "motorComplect.h"

using namespace trikControl;

MotorComplect::MotorComplect(Motor *motor, Encoder *motorEncoder, int const &complectID)
	: mMotor(motor)
	, mEncoder(motorEncoder)
	, mPower(0)
	, mIncrement(4)
	, mID(complectID)
	, mIsReversed(false)
	, mLatestEncoderVal(0)
	, mHistoryPointer(0)
{
}

void MotorComplect::setMotor(trikControl::Motor *motor)
{
	mMotor = motor;
}

void MotorComplect::setEncoder(trikControl::Encoder *motorEncoder)
{
	mEncoder = motorEncoder;
}

trikControl::Motor* MotorComplect::motor()
{
	return mMotor;
}

trikControl::Encoder* MotorComplect::encoder()
{
	return mEncoder;
}

float MotorComplect::readEncoder()
{
	if (mEncoder == nullptr)
	{
		qDebug() << "nullptr encoder requested";
		return -1;
	}
	return mEncoder->read();
}

void MotorComplect::resetEncoder()
{
	mEncoder->reset();
}

void MotorComplect::clearHistory()
{
	mDiffHistory.clear();
	mPureHistory.clear();
	mHistoryPointer = 0;
}

void MotorComplect::setMotorPower(int power)
{
	power = qMin(power, 100);
	power = qMax(power, -100);
	mMotor->setPower(power);
	mPower = power;
}

void MotorComplect::setIncrement(int const &increment)
{
	mIncrement = increment;
}

void MotorComplect::setReversed(bool const &isReversed)
{
	mIsReversed = isReversed;
}

void MotorComplect::increaseSpeed()
{
	setMotorPower(mPower + ((mIsReversed)? mIncrement : -mIncrement));
}

void MotorComplect::decreaseSpeed()
{
	setMotorPower(mPower + ((mIsReversed)? -mIncrement : mIncrement));
}

void MotorComplect::updateHistory()
{
	float const current = mEncoder->read();
	mDiffHistory.append(mLatestEncoderVal - current);
	mLatestEncoderVal = current;
	mPureHistory.append(current);
}

void MotorComplect::completeHistory()
{
	saveHistoryToFile("record");
}

void MotorComplect::startPlayback()
{
	mHistoryPointer = 0;
	for (int i = 0; i < forecastRange + 1; i++)
	{
		mDiffHistory.append(0);
	}
}

void MotorComplect::completePlayback()
{
	saveHistoryToFile("playback");
}

void MotorComplect::adjustSpeed()
{
	float const current = mEncoder->read();
	float const diff = current - mLatestEncoderVal;
	mLatestEncoderVal = current;
	float const nextDiff = forecastNextValue();

	//rewriting history
	mPureHistory.at(mHistoryPointer) = current;
	mDiffHistory.at(mHistoryPointer) = diff;

	//qDebug() << "curr :: correct" << currentValue << " " << correctValue;
	if (qAbs(diff - nextDiff) < epsilon)
	{
		return;
	}

	if (diff < nextDiff)
	{
		increaseSpeed();
	}
	else
	{
		decreaseSpeed();
	}
}

float MotorComplect::forecastNextValue()
{
	int const histrorySize = mDiffHistory.size();
	if (mHistoryPointer >= histrorySize)
	{
		emit playbackDone();
		return mDiffHistory.last();
	}

	while (mHistoryPointer >= histrorySize - lookForwardDistance)
	{
		lookForwardDistance--;
	}

	float result = 0;
	int totalWeight = 0;
	for (int i = 0; i < lookForwardDistance; i++)
	{
		result += mDiffHistory.at(mHistoryPointer + i) * (i + 1);
		totalWeight += i + 1;
	}
	mHistoryPointer++;
	return result / totalWeight;
}

void MotorComplect::saveHistoryToFile(QString &comment)
{
	QString filename = "motorComplect_" + QString::number(mID) + "--"
			+ comment
			+ QTime::currentTime().toString("hh-mm-ss-zzz") + ".txt";
	QFile file(filename);
	if (!file.open(QFile::WriteOnly))
	{
		qDebug() << "could not create file";
		return;
	}
	file.write(QString("pure data:\n").toUtf8());
	QString line("");
	for (int i = 0; i < mPureHistory.size(); i++)
	{
		line = QString::number(mPureHistory.at(i), 'f', 2) + " \t"
				+ QString::number(mDiffHistory.at(i), 'f', 2) + "\n";
		file->write(line.toUtf8());
	}
	file.flush();
	file.close();
}
