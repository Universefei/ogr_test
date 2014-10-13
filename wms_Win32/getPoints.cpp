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
	retPoints.numPoints = 0;
	retPoints.pCoords = NULL;
	retPoints.numRings = 0;
	retPoints.RingSep = NULL;

    OGRGeometry* pGeom = pFeature->GetGeomFieldRef(0);
	OGRwkbGeometryType eGeomType = pGeom->getGeometryType();

    switch (eGeomType)
    {
        case wkbPoint:
			{
				OGRPoint* pPoint = dynamic_cast< OGRPoint* >(pGeom);
				if(!pPoint) break;
				retPoints.pCoords = new scrCoord[1];
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
				retPoints.numRings = inRingCnt+1;
				retPoints.RingSep = new int[retPoints.numRings];


				int inRing = 0;
				for(inRing = 0; inRing < inRingCnt; ++inRing)
				{
					pointCnt +=  pPolygon->getInteriorRing(inRing)->getNumPoints();
				}
				retPoints.numPoints = pointCnt; 
				retPoints.pCoords = new scrCoord[pointCnt];

				// Geting points from exterior Ring of Polygon
				int maxNum = pExtRing->getNumPoints();
				retPoints.RingSep[0] = maxNum;
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
					retPoints.RingSep[inRing+1] = inRingPointCnt;
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
		case wkbMultiLineString:
			{
				OGRMultiLineString* pMLString = dynamic_cast<OGRMultiLineString*>(pGeom);
				if(!pMLString) break;
				int linesCnt = pMLString->getNumGeometries();
				retPoints.numRings = linesCnt;
				retPoints.RingSep = new int[retPoints.numRings];
				
				for(int st=0; st<linesCnt; ++st)
				{
					OGRGeometry* p=pMLString->getGeometryRef(st);
					OGRLineString* pLine = dynamic_cast<OGRLineString*>(p);
					retPoints.numPoints += pLine->getNumPoints();
				}

				retPoints.pCoords = new scrCoord[retPoints.numPoints];

				int pointCntr = 0;
				for(int wtf=0; wtf < linesCnt; ++wtf)
				{
					OGRGeometry* pgeom = pMLString->getGeometryRef(wtf);
					OGRLineString* pLString = dynamic_cast<OGRLineString*>(pgeom);
					int pointCnt = pLString->getNumPoints();
					retPoints.RingSep[wtf]=pointCnt;
					for(int i=0;i<pointCnt;++i)
					{
						(retPoints.pCoords)[pointCntr].x = pLString->getX(i);
						(retPoints.pCoords)[pointCntr].y = pLString->getY(i);
						(retPoints.pCoords)[pointCntr].z = pLString->getZ(i);
						++pointCntr;
					}
				}

				break;
			}

		case wkbMultiPolygon:
			{
				OGRMultiPolygon* pMPolygon = dynamic_cast<OGRMultiPolygon*>(pGeom);
				if(!pMPolygon) break;

				//
				// Countting how many Rings and how many points.
				//
				int polygonCnt = pMPolygon->getNumGeometries();
				retPoints.numRings = polygonCnt;
				retPoints.numPoints = 0;
				for(int c=0; c<polygonCnt; ++c)
				{
					OGRPolygon* pPolygon = dynamic_cast<OGRPolygon*>(pMPolygon->getGeometryRef(c));
					if(!pPolygon) break;
					int interiorCnt = pPolygon->getNumInteriorRings();
					retPoints.numRings += interiorCnt;
					retPoints.numPoints += pPolygon->getExteriorRing()->getNumPoints();
					for(int cc=0; cc<interiorCnt; ++cc)
					{
						retPoints.numPoints += pPolygon->getInteriorRing(cc)->getNumPoints();
					}
				}

				retPoints.pCoords = new scrCoord[retPoints.numPoints];
				retPoints.RingSep = new int[retPoints.numRings];

				cout<<"allocate over"<<endl;
				
				//
				// Dumping points of rings.
				//
				int pointLooker = 0;
				int ringLooker = 0;
				for(int j=0; j < polygonCnt; ++j)
				{
					OGRPolygon* pPolygon = dynamic_cast<OGRPolygon*>(pMPolygon->getGeometryRef(j));
					if(!pPolygon) break;

					// Geting points from exterior Ring of Polygon
					OGRLineString* pExtRing = pPolygon->getExteriorRing();
					int maxNum = pExtRing->getNumPoints();
					retPoints.RingSep[ringLooker] = maxNum;
					++ringLooker;
					int counter = 0;
					for(; counter < maxNum; ++counter)
					{
						(retPoints.pCoords)[pointLooker].x = pExtRing->getX(counter);
						(retPoints.pCoords)[pointLooker].y = pExtRing->getY(counter);
						(retPoints.pCoords)[pointLooker].z = pExtRing->getZ(counter);
						++pointLooker;
					}

					cout<<"exterior over"<<endl;
					// Geting points from interior Rings of Polygon
					int inRingCnt = pPolygon->getNumInteriorRings();
					
					for(int inRing = 0; inRing < inRingCnt; ++inRing)
					{
						OGRLineString* pInRing = pPolygon->getInteriorRing(inRing);
						int inRingPointCnt = pInRing->getNumPoints();
						retPoints.RingSep[ringLooker] = inRingPointCnt;
						++ringLooker;
						for(int t=0; t < inRingPointCnt; ++t)
						{
							(retPoints.pCoords)[pointLooker].x = pInRing->getX(t);
							(retPoints.pCoords)[pointLooker].y = pInRing->getY(t);
							(retPoints.pCoords)[pointLooker].z = pInRing->getZ(t);
							++counter;
						}
					}
				}
				break;
			}
        default:
			{
				break;
			}

    }
	return retPoints;
}

void destroyPointsStr(struct pointStr* pPStr)
{
	if(pPStr->pCoords)
	{
		delete[] pPStr->pCoords;
		pPStr->pCoords = NULL;
	}
	if(pPStr->RingSep)
	{
		delete[] pPStr->RingSep;
		pPStr->RingSep = NULL;
	}
}






