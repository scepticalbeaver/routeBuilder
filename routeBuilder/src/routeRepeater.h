#pragma once

#include <QDebug>

#include "trackStorage.h"

class RouteRepeater : public QObject
{
	Q_OBJECT
public:
	explicit RouteRepeater(QVector<MotorComplect *> *complects, TrackStorage *storage, QObject *parent = 0);
	~RouteRepeater() {}

	void playback();

signals:
	void playbackDone();

protected:
	static int const timeout = 50; //ms
	static float constexpr recommendVelocity = 0.4; // meters per sec
	TrackStorage *mStorage;
	QVector<MotorComplect *> *mSharedComplects;
	QMap<int, MotorComplect *> mMotorComplects;
	QTimer mTimer;
	int mHistoryPointer;
	int mHistorySize;

	void initTimer();
	void fetchMotors();
	void stopMotors();
	void moveToNextPoint();
	bool hasReverseVelocity(int const id);
	bool hasReachedPosition(float const &currentPos, float const finPos, bool const &isReversed);
	float valueAtTimePos(int const &id, int const &pointer);

protected slots:
	void adjustMotors();
	void playbackStopped();
};

