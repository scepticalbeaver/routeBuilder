#pragma once

namespace keywords
{
//enum class TrackingFlows
//{
//	mainTrackingFlow = 0
//	, alternativeFlow
//};

class TrackingFlows
{
public:
	static int constexpr mainTrackingFlow()
	{
		return 0;
	}
	static int constexpr alternativeFlow()
	{
		return 1;
	}
};
}


