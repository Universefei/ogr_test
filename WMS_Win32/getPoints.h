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
	int* RingSep;
	int numRings;
};

struct pointStr
getPoints(OGRFeature* pFeature);

void destroyPointsStr(pointStr*);










