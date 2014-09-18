#include "StdAfx.h"
#include "ogr_core.h" // OGRwkbGeometryType
#include "ogr_geometry.h" //
#include "getPoints.h"

#include <iostream>
#include <vector>

using namespace std;

struct pointStr
getPoints(OGRFeature* pFeature)
{
	struct pointStr retPoints;
    OGRGeometry* pGeom = pFeature->GetGeomFieldRef(0);
	OGRwkbGeometryType eGeomType = pGeom->getGeometryType();

    switch (eGeomType)
    {
        case wkbPoint:
			{
				OGRPoint* pPoint = dynamic_cast< OGRPoint* >(pGeom);
				if(!pPoint) break;
				retPoints.pCoords = new scrCoord;
				(retPoints.pCoords)->x = pPoint->getX();
				(retPoints.pCoords)->y = pPoint->getY();
				(retPoints.pCoords)->z = pPoint->getZ();
				retPoints.numPoints = 1;
				break;
			}

        case wkbLineString:
			{
				OGRLineString* pLString = dynamic_cast<OGRLineString*>(pGeom);
				if(!pLString) break;
				int pointCnt = pLString->getNumPoints();
				retPoints.pCoords = new scrCoord[pointCnt];
				for(int i=0;i<pointCnt;++i)
				{
					(retPoints.pCoords)[i].x = pLString->getX(i);
					(retPoints.pCoords)[i].y = pLString->getY(i);
					(retPoints.pCoords)[i].z = pLString->getZ(i);
				}
				retPoints.numPoints = pointCnt;
				break;
			}

        case wkbPolygon:
			{
				OGRPolygon* pPolygon = dynamic_cast<OGRPolygon*>(pGeom);
				if(!pPolygon) break;

				// Counting Points amount to alloc heap memory
				int pointCnt = 0;
				OGRLineString* pExtRing = pPolygon->getExteriorRing();
				pointCnt += pExtRing->getNumPoints();
				int inRingCnt = pPolygon->getNumInteriorRings();
				int inRing = 0;
				for(inRing = 0; inRing < inRingCnt; ++inRing)
				{
					pointCnt +=  pPolygon->getInteriorRing(inRing)->getNumPoints();
				}
				retPoints.numPoints = pointCnt; 
				retPoints.pCoords = new scrCoord[pointCnt];

				// Geting points from exterior Ring of Polygon
				int maxNum = pExtRing->getNumPoints();
				int counter = 0;
				for(; counter < maxNum; ++counter)
				{
					(retPoints.pCoords)[counter].x = pExtRing->getX(counter);
					(retPoints.pCoords)[counter].y = pExtRing->getY(counter);
					(retPoints.pCoords)[counter].z = pExtRing->getZ(counter);
				}

				// Geting points from interior Rings of Polygon
				for(inRing = 0; inRing < inRingCnt; ++inRing)
				{
					OGRLineString* pInRing = pPolygon->getInteriorRing(inRing);
					int inRingPointCnt = pInRing->getNumPoints();
					for(int t; t < inRingPointCnt; ++t)
					{
						(retPoints.pCoords)[counter].x = pInRing->getX(t);
						(retPoints.pCoords)[counter].y = pInRing->getY(t);
						(retPoints.pCoords)[counter].z = pInRing->getZ(t);
						++counter;
					}
				}
				break;
			}
        case wkbMultiPoint:
			{
				break;
			}
        default:
			{
				break;
			}
    }
	return retPoints;
}

void Destroy(pointStr* pPStr)
{
	delete[] pPStr->pCoords;
	pPStr->pCoords = NULL;
}
