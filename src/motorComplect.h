#pragma once

#include <QVector>
#include <QDebug>
#include <QFile>
#include <QTime>

#include "trikControl/brick.h"

class MotorComplect : public QObject
{
	Q_OBJECT
public:
	MotorComplect(trikControl::Motor *motor, trikControl::Encoder *motorEncoder, int const &complectID);
	void setMotor(trikControl::Motor *motor);
	void setEncoder(trikControl::Encoder *motorEncoder);
	trikControl::Motor* motor();
	trikControl::Encoder* encoder();
	float readEncoder();
	void resetEncoder();
	void clearHistory();

	void setMotorPower(int power);
	void setIncrement(int const &increment);
	void setReversed(bool const &isReversed);
	void increaseSpeed();
	void decreaseSpeed();

public slots:
	void updateHistory();
	void completeHistory();
	void startPlayback();
	void completePlayback();
	void adjustSpeed();

signals:
	void playbackDone();

protected:
	static int const epsilon = 2;
	static int const forecastRange = 2;
	trikControl::Motor *mMotor;
	trikControl::Encoder *mEncoder;
	int mPower;
	int mIncrement;
	int const mID;
	bool mIsReversed;
	float mLatestEncoderVal;
	int mHistoryPointer;
	QVector<float> mDiffHistory;
	QVector<float> mPureHistory;

	float forecastNextValue();
	void saveHistoryToFile(QString &comment = QString(""));
};

