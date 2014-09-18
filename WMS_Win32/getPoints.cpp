#include "StdAfx.h"
#include "ogr_core.h" // OGRwkbGeometryType
#include "ogr_geometry.h" //
#include "getPoints.h"

#include <iostream>
#include <vector>

using namespace std;


void getPoint(OGRFeature* pFeature, int* numPoints, scrCoord** pCoords)
{


    OGRGeometry* pGeom = pFeature->GetGeomFieldRef(0);
	OGRwkbGeometryType eGeomType = pGeom->getGeometryType();

    switch (eGeomType)
    {
        case wkbPoint:
            OGRPoint* pPoint = dynamic_cast<OGRPoint*>(pGeom);
            if(!pPoint) break;
            *pCoords = new scrCoord;
            (*pCoords)->x = pPoint->getX();
            (*pCoords)->y = pPoint->getY();
            (*pCoords)->z = pPoint->getZ();
            *numPoints = 1;
            break;

        case wkbLineString:
            OGRLineString* pLString = dynamic_cast<OGRLineString*>(pGeom);
            if(!pLString) break;
            int pointCnt = pLString->getNumPoints();
            *pCoords = new scrCoord[pointCnt];
            for(int i=0;i<pointCnt;++i)
            {
                (*pCoords)[i]->x = pGeom->getX(i);
                (*pCoords)[i]->y = pGeom->getY(i);
                (*pCoords)[i]->z = pGeom->getZ(i);
            }
            numPoints = pointCnt;
            break;

        case wkbPolygon:
            OGRPolygon* pPolygon = dynamic_cast<OGRPolygon*>(pGeom);
            if(!pPolygon) break;

            // Counting Points amount to alloc heap memory
            int pointCnt = 0;
            OGRLineString* pExtRing = pPolygon->getExteriorRing();
            pointCnt += pExtRing->getNumPoints();
            int inRingCnt = pPolygon->getNumInteriorRings();
            for(int inRing = 0; inRing < inRingCnt; ++inRing)
            {
                pointCnt +=  pPolygon->getInteriorRing(inRing)->getNumPoints();
            }
            *numPoints = pointCnt; 
            *pCoords = new scrCoord[pointCnt];

            // Geting points from exterior Ring of Polygon
            int maxNum = pExtRing->getNumPoints();
            int counter = 0;
            for(; counter < maxNum; ++counter)
            {
                (*pCoords)[counter]->x = pGeom->getX(counter);
				(*pCoords)[counter]->y = pGeom->getY(counter);
                (*pCoords)[counter]->z = pGeom->getZ(counter);
            }

            // Geting points from interior Rings of Polygon
            for(int inRing = 0; inRing < inRingCnt; ++inRing)
            {
                OGRLineString* pInRing = pPolygon->getInteriorRing(inRing);
                int inRingPointCnt = pInRing->getNumPoints();
                for(int t; t < inRingPointCnt; ++t)
                {
                    (*pCoords)[counter]->x = pInRing->getX(t);
                    (*pCoords)[counter]->y = pInRing->getY(t);
                    (*pCoords)[counter]->z = pInRing->getZ(t);
                    ++counter;
                }
            }
            break;
        case wkbMultiPoint:
            break;
        default:
            break;
    }
}

void Destroy()
{
}
