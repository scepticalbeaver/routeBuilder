#include "motorComplect.h"

using namespace trikControl;

MotorComplect::MotorComplect(Motor *motor, Encoder *motorEncoder, int const &complectID)
	: mMotor(motor)
	, mEncoder(motorEncoder)
	, mPower(0)
	, mIncrement(1)
	, mID(complectID)
	, mIsReversed(false)
	, mIsMotorBlocked(false)
	, mLatestEncoderVal(0)
	, mHistoryPointer(0)
	, auxTime(25)
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
	if (mIsMotorBlocked)
	{
		return;
	}
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
	setMotorPower(mPower + ((mIsReversed)? -mIncrement : mIncrement));
}

void MotorComplect::decreaseSpeed()
{
	setMotorPower(mPower + ((mIsReversed)? mIncrement : -mIncrement));
}

void MotorComplect::updateHistory()
{
	float const current = mEncoder->read();
	mDiffHistory.append(current - mLatestEncoderVal);
	mLatestEncoderVal = current;
	mPureHistory.append(current);
}

void MotorComplect::completeHistory()
{
	saveHistoryToFile(QString("record"));
}

void MotorComplect::startPlayback()
{
	mHistoryPointer = 0;
	mEncoder->reset();
	mLatestEncoderVal = 0;
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

	if (auxTime++ >= 120)
	{
		auxTime = 0;
		float percent = (float)mHistoryPointer / ((float)mDiffHistory.size()) ;
		qDebug() << "done: " << mHistoryPointer << mDiffHistory.size() << "\t" << percent;
		qDebug() << "curr :: correct\t" << current << "\t" << mPureHistory.at(mHistoryPointer);
		qDebug() << "diff :: forecast\t" << diff << "\t" << nextDiff << "\n";
	}

	//rewriting history
	mPureHistory.replace(mHistoryPointer - 1, current);
	mDiffHistory.replace(mHistoryPointer - 1, diff);


	if (qAbs(diff - nextDiff) < epsilon)
	{
		return;
	}

	if (nextDiff == 0)
	{
		setMotorPower(0);
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
		setMotorPower(0);
		mIsMotorBlocked = true;
		emit playbackDone();
		return mDiffHistory.last();
	}
	int lookForwardDistance = forecastRange;

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

void MotorComplect::saveHistoryToFile(QString comment)
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
		file.write(line.toUtf8());
	}
	file.flush();
	file.close();
}
