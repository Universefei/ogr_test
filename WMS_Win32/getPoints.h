#include "ogr_feature.h"

typedef struct scrCoord
{
    double x;
    double y;
    double z;
};

void getPoint(OGRFeature* pFeature, int* numPoints, struct scrCoord** pCoords);
void destroy();