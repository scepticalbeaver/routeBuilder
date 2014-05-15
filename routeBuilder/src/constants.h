#pragma once

namespace keywords
{
class TrackingFlows
{
public:
	static int const mainTrackingFlow = 0;
	static int const alternativeFlow = 1;
};
}

namespace robotConstants
{
//! @class RobotSpecifications stores various external measurements of robot in [meters]
class RobotSpecifications
{
public:
	//! width between left and right wheels
	static double constexpr wheelBase = 0.30;
	//! whhel diameter needs to calc traveled distance
	static constexpr float wheelDiameter = 0.2;
	//! difference of encoder values per one full round
	static constexpr float encodersPerRound = 410;

	static constexpr float maxDiffPerMS = 12 / 10;
	static double pi();
};
}





