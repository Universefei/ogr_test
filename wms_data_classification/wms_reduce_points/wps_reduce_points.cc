#include <ogrsf_frmts.h>
#include <ogr_core.h> /* OGRwktGeomType */
#include <iostream>
using namespace std;

OGRGeometry* blurGeom( OGRGeometry* pGeom);

int main (int argc, char const* argv[])
{
    OGRSFDriver         *pPGDriver = NULL;
    OGRSFDriver         *pGJDriver = NULL;
    OGRSFDriver         *pSHDriver = NULL;

    OGRDataSource       *pPGDS = NULL;
    OGRDataSource       *pGJDS = NULL;
    OGRDataSource       *pSHDS = NULL;

    OGRLayer            *pPGLayer = NULL;
    int                 nLayers = 0;

    /* --------------------------------------------------------------------- */
    /*     Connect PostGIS ultilizing OGR                                    */
    /* --------------------------------------------------------------------- */
    OGRRegisterAll();

    pPGDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName("PostgreSQL");
    pGJDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName("GeoJSON");
    pSHDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName("Shapefile");
    if(!pPGDriver)
    {
        fprintf(stderr,"GetDriver PostgreSQL error \n");
        exit(1);
    }

    //const char*         pszPGConnInfo = "PG: host='192.168.3.201' port='5432' user='postgres' password='postgres' dbname='us_osm'";
    const char* pszPGConnInfo = "PG: host='192.168.1.99' port='5432' user='postgres' password='postgres' dbname='dc'";
    try
    {
        pPGDS = pPGDriver->Open(pszPGConnInfo);
    }
    catch(...)
    {
        fprintf(stderr,"Connect PostgreSQL error\n");
    }

    if(!pPGDS)
    {
        fprintf(stderr,"Connect PostgreSQL error\n");
        exit(1);
    }

    /* --------------------------------------------------------------------- */
    /*     Dump table to OGRLayer and create output files                    */
    /*     every file for every OGRLayer                                     */
    /* --------------------------------------------------------------------- */
    nLayers = pPGDS->GetLayerCount();
    int iLyr = 0;
    char* pszSRSWkt = NULL;
    OGRLayer *pOLayer = NULL;
    OGRFeature *pTmpFeature = NULL;
    for(; iLyr<nLayers; ++iLyr)
    {
        pPGLayer = pPGDS->GetLayer(iLyr);
        cout<<"Layer "<<iLyr<<": "<<pPGLayer->GetName()<<endl;
        pGJDS = pGJDriver->CreateDataSource(pPGLayer->GetName());
        pOLayer = pGJDS->CreateLayer( pPGLayer->GetName(), pPGLayer->GetSpatialRef() );

    /* --------------------------------------------------------------------- */
    /*     Do reduce points                                                  */
    /* --------------------------------------------------------------------- */
        int nFeature = pPGLayer->GetFeatureCount();
        int iFea = 0;
        for(; iFea< nFeature; ++iFea)
        {
            pTmpFeature = pPGLayer->GetNextFeature();
            if( !pTmpFeature )
                break;

            OGRGeometry* pTmpGeom = NULL;
            OGRGeometry* pBluredGeom = NULL;
            int nGeom = pPGLayer->GetLayerDefn()->GetGeomFieldCount();
            int iGeom = 0;
            OGRwkbGeometryType eGeomType;
            for( ; iGeom < nGeom; ++iGeom )
            {
                pTmpGeom = pTmpFeature->GetGeomFieldRef(iGeom);
                eGeomType = pTmpGeom->getGeometryType();

                //TODO: reduce points;
                pBluredGeom = blurGeom( pTmpGeom );
                pTmpFeature->SetGeomFieldDirectly( iGeom, pBluredGeom );
            }

            pOLayer->CreateFeature(pTmpFeature);
            OGRFeature::DestroyFeature(pTmpFeature);
            pTmpFeature = NULL;
        }

        OGRDataSource::DestroyDataSource(pGJDS);
        pGJDS = NULL;

    }
    return 0;
}


OGRGeometry* blurGeom( OGRGeometry* pGeom)
{
    OGRGeometry* pNewGeom;
    OGRwkbGeometryType eGeomType = pGeom->getGeometryType();
    OGRSpatialReference* pSRS = pGeom->getSpatialReference();

    switch (eGeomType)
    {
        case wkbPoint:
			{
                pNewGeom = OGRGeometryFactory::createGeometry(wkbPoint);
                OGRPoint* pRetGeom = dynamic_cast< OGRPoint* >(pGeom);
                pNewGeom->assignSpatialReference(pSRS);

				OGRPoint* pPoint = dynamic_cast< OGRPoint* >(pGeom);
				if(!pPoint) break;

                pRetGeom->setX( pPoint->getX() );
                pRetGeom->setY( pPoint->getY() );
                pRetGeom->setZ( pPoint->getZ() );
				break;
			}

        case wkbLineString:
			{
                pNewGeom = OGRGeometryFactory::createGeometry(wkbPoint);
                OGRLineString* pRetGeom = dynamic_cast< OGRLineString* >(pGeom);
                pNewGeom->assignSpatialReference(pSRS);

				OGRLineString* pLString = dynamic_cast<OGRLineString*>(pGeom);
				if(!pLString) break;

				int pointCnt = pLString->getNumPoints();
				for(int i=0; i<pointCnt; i+9)
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
}
