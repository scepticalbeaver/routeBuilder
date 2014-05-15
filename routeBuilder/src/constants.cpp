#include "constants.h"

#include <qmath.h>

using namespace keywords;
using namespace robotConstants;

int const TrackingFlows::mainTrackingFlow;
int const TrackingFlows::alternativeFlow;


double RobotSpecifications::pi()
{
	return 4 * qAtan(1);
}
