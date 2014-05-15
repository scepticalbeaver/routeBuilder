#pragma once

#include <QDebug>

#include "trackStorage.h"

//! @class RouteRepeater playbacks robot track by controlling its motors
class RouteRepeater : public QObject
{
	Q_OBJECT
public:
	RouteRepeater(QVector<MotorComplect *> *complects, TrackStorage *storage, QObject *parent = 0);
	~RouteRepeater() {}

	//! @brief playback start repeating track
	//! @param trackFlow number of track from TrackStorage
	void playback(int const &trackFlow);

signals:
	void playbackDone();

protected:
	//! period  of speed correction and map-checking
	static int const timeout = 50; //ms
	static float constexpr recommendVelocity = 0.4; // meters per sec
	static int const isBlockedMotor = -1;
	TrackStorage *mStorage;
	QVector<MotorComplect *> *mSharedComplects;
	QMap<int, MotorComplect *> mMotorComplects;
	QTimer mTimer;
	int mHistoryPointer;
	int mHistorySize;
	int mPlaybackFlow;

	void initTimer();
	void fetchMotors();
	void stopMotors();

	//! chooses to which point we must move next time
	void moveToNextPoint(QMap<MotorComplect *, float> &encoderValues);

	bool hasReachedPosition(float const &currentPos, float const finPos, bool const &isReversed);
	bool isNegativePointsDiff(int const &id, int const &from, int const &to);
	bool isNextPointBackwards(int const &id);
	bool isCurrentPointBackwards(int const &id);
	bool hasReverseVelocity(int const &id, float const &currentValue);

	float valueAtTimePos(int const &id, int const &pointer);

	QMap<MotorComplect *, float> fetchCurrEncoderValues();
	//! finds wheels which must stay on theirs position
	QMap<MotorComplect *, float> blockExcessMotors(QMap<MotorComplect *, float> &encoderValues);

protected slots:
	void adjustMotors();
	void playbackStopped();
};

