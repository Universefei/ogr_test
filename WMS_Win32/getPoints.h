#include "ogr_feature.h"

struct scrCoord
{
    double x;
    double y;
    double z;
};

struct pointStr
{
	int numPoints;
	struct scrCoord* pCoords;
};

struct pointStr
getPoints(OGRFeature* pFeature);

void destroy(pointStr*);