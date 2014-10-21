#include <ogrsf_frmts.h>
#include <ogr_core.h> /* OGRwktGeomType */
#include <iostream>
#include "config.h"
using namespace std;

#define POINTLEAP 5
#define DEBUG std::cout<<__LINE__<<std::endl

/******************************************************************************
 *                          blurLineString                                    *
 *****************************************************************************/

/**
 * T->OGRLineString / OGRLinearRing
 */
template<typename T>
T* blurLineString(T *pGeom)
{
    /**
     * do not new OGRLineString directly
     * because, feature delete it using gdal dynamic lib run time.
     */

    if(pGeom->getGeometryType() != wkbLineString && pGeom->getGeometryType() != wkbLinearRing)
        return NULL;

    /* verbose info */
    cout<<"Before blurLineString:"<<endl;
    char* pszWktGeom = NULL;
    pGeom->exportToWkt(&pszWktGeom);
    cout<<pszWktGeom<<endl;
    CPLFree(pszWktGeom);
    pszWktGeom = NULL;

    T* pRetGeom = new T();

    OGRSpatialReference* pSRS = pGeom->getSpatialReference();
    pRetGeom->assignSpatialReference(pSRS);

    int pointCnt = pGeom->getNumPoints();
    OGRPoint* pTmpPoint = dynamic_cast< OGRPoint* >(
            OGRGeometryFactory::createGeometry(wkbPoint) );

    int i = 0;
    //if(pGeom->getGeometryType() == wkbLinearRing)
    if(pointCnt >= 3)
    {
        /* Must have over 4 points, and the start point equal the last one */
        pGeom->getPoint( 0, pTmpPoint );
        pRetGeom->addPoint( pTmpPoint );
        pGeom->getPoint( 1, pTmpPoint );
        pRetGeom->addPoint( pTmpPoint );
        pGeom->getPoint( 2, pTmpPoint );
        pRetGeom->addPoint( pTmpPoint );
        i += 3;
    }

    for(; i<pointCnt; i+=POINTLEAP)
    {
        pGeom->getPoint( i, pTmpPoint );
        pRetGeom->addPoint( pTmpPoint );
    }

    OGRPoint* pLastPoint = dynamic_cast< OGRPoint* >(
            OGRGeometryFactory::createGeometry(wkbPoint) );
    pGeom->getPoint( pointCnt-1, pLastPoint );
    if( pLastPoint != pTmpPoint )
    {
        pRetGeom->addPoint( pLastPoint );
    }

    /* verbose info */
    cout<<"After blurLineString:"<<endl;
    pRetGeom->exportToWkt(&pszWktGeom);
    cout<<pszWktGeom<<endl;
    CPLFree(pszWktGeom);
    pszWktGeom = NULL;

    cout<<"before return"<<endl;
    return pRetGeom;
}


/******************************************************************************
 *                             blurPolygon                                    *
 *****************************************************************************/

OGRPolygon* blurPolygon(OGRPolygon *pGeom)
{
    /**
     * do not new OGRPolygon directly
     * because, feature delete it using gdal dynamic lib run time.
     */
    OGRGeometry* pNewGeom = OGRGeometryFactory::createGeometry( wkbPolygon );
    OGRPolygon* pRetGeom = dynamic_cast< OGRPolygon* >(pNewGeom);
    if(!pRetGeom)
        cout<<"error to cast to OGRPolygon"<<endl;

    OGRSpatialReference* pSRS = pGeom->getSpatialReference();
    pRetGeom->assignSpatialReference(pSRS);

    // exterior Ring of Polygon
    OGRLinearRing* pExtRing = pGeom->getExteriorRing();
    OGRLinearRing* pBluredExtRing = blurLineString(pExtRing);
    pRetGeom->addRingDirectly( pBluredExtRing );

    // interior Rings of Polygon
    int iRing = 0;
    int nRingCnt = pGeom->getNumInteriorRings();
    for( ; iRing < nRingCnt; ++iRing)
    {
        OGRLinearRing* pInRing = pGeom->getInteriorRing(iRing);
        OGRLinearRing* pBluredInRing = blurLineString(pInRing);
        OGRLinearRing* pLinearRing = dynamic_cast< OGRLinearRing* >(pBluredInRing);

        pRetGeom->addRingDirectly( pLinearRing );
    }

    return pRetGeom;
}

/******************************************************************************
 *                               blurGeom                                     *
 *****************************************************************************/

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
                OGRPoint* pRetGeom = dynamic_cast< OGRPoint* >(pNewGeom);
                pNewGeom->assignSpatialReference(pSRS);

				OGRPoint* pPoint = dynamic_cast< OGRPoint* >(pGeom);
				if(!pPoint) break;

                pRetGeom->setX( pPoint->getX() );
                pRetGeom->setY( pPoint->getY() );
                pRetGeom->setZ( pPoint->getZ() );
                return pRetGeom;
			}

        case wkbLineString:
			{
				OGRLineString* pLString = dynamic_cast<OGRLineString*>(pGeom);
				if(!pLString) break;

                OGRLineString* pRetGeom = blurLineString(pLString);
                return pRetGeom;
			}

        case wkbPolygon:
			{
				OGRPolygon* pPolygon = dynamic_cast<OGRPolygon*>(pGeom);
				if(!pPolygon) break;

                OGRPolygon* pRetGeom = blurPolygon(pPolygon);
                return pRetGeom;
			}

		case wkbMultiLineString:
			{
				OGRMultiLineString* pMLString = dynamic_cast<OGRMultiLineString*>(pGeom);
				if(!pMLString) break;

                OGRGeometry* pNewGeom = OGRGeometryFactory::createGeometry(wkbMultiLineString);
                OGRMultiLineString* pRetGeom = dynamic_cast< OGRMultiLineString* >(pNewGeom);
				
				int linesCnt = pMLString->getNumGeometries();
				for(int st=0; st<linesCnt; ++st)
				{
					OGRGeometry* p=pMLString->getGeometryRef(st);
					OGRLineString* pLine = dynamic_cast<OGRLineString*>(p);
                    
                    OGRLineString* poBluredLS = blurLineString(pLine);
                    pRetGeom->addGeometryDirectly(poBluredLS);
				}
                return pRetGeom;
			}

		case wkbMultiPolygon:
			{
				OGRMultiPolygon* pMPolygon = dynamic_cast<OGRMultiPolygon*>(pGeom);
				if(!pMPolygon) break;

                OGRGeometry* pNewGeom = OGRGeometryFactory::createGeometry(wkbMultiPolygon);
                OGRMultiPolygon* pRetGeom = dynamic_cast< OGRMultiPolygon* >(pNewGeom);

				int polygonCnt = pMPolygon->getNumGeometries();
				for(int c=0; c<polygonCnt; ++c)
				{
					OGRPolygon* pPolygon = dynamic_cast<OGRPolygon*>(pMPolygon->getGeometryRef(c));
					if(!pPolygon) break;
                    OGRPolygon* poBluredP = blurPolygon(pPolygon);
                    pRetGeom->addGeometryDirectly(poBluredP);
				}
                return pRetGeom;
			}
        default: 
            return NULL;
    }
}

/******************************************************************************
 *                               main()                                       *
 *****************************************************************************/
                        
int main (int argc, char const* argv[])
{
    OGRSFDriver         *pPGDriver = NULL;
    OGRSFDriver         *pDriver = NULL;

    OGRDataSource       *pPGDS = NULL;
    OGRDataSource       *poODS = NULL;

    OGRLayer            *pPGLayer = NULL;
    int                 nLayers = 0;

    /* --------------------------------------------------------------------- */
    /*     Connect PostGIS ultilizing OGR                                    */
    /* --------------------------------------------------------------------- */
    OGRRegisterAll();

    pPGDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName("PostgreSQL");
    if(!pPGDriver)
    {
        fprintf(stderr,"GetDriver PostgreSQL error \n");
        exit(1);
    }

    /* Env in Lab. */
    /* const char* pszPGConnInfo = "PG: host='192.168.1.99' port='5432' \ */
    /*                              user='postgres' password='postgres' dbname='dc'"; */

    /* Env on macbook air */
    const char* pszPGConnInfo = "PG: host='219.244.118.170' port='5432' \
                                 user='postgres' password='postgres' dbname='dc'";
    /* const char* pszPGConnInfo = "PG: host='10.61.125.55' port='5432' \ */
    /*                              user='postgres' password='postgres' dbname='dc'"; */
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
    DEBUG;

#if defined(DUMPTOPG)
    const char* pszOutConnInfo = "PG: host='192.168.1.99' \
                                  port='5432' user='postgres' \
                                  password='postgres' dbname='blur_dc'";
    /* Try to open a exsit dbname */
    poODS = pPGDriver->Open(pszOutConnInfo);
    /* If no that dbname, try to create one */
    if(!pooDS)
    {
        poODS = pPGDriver->CreateDataSource(pszOutConnInfo);
    }
    if(!poODS)
    {
        cout<<"conn PG blur_dc error"<<endl;
    }
#elif defined(DUMPTOSHP)
    pDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName("ESRI Shapefile");
    DEBUG;
#elif defined(DUMPTOGML)
    pDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName("GML");
#else
    pDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName("GeoJSON");
#endif

    DEBUG;
    /* --------------------------------------------------------------------- */
    /*     Dump table to OGRLayer and create output files                    */
    /*     every file for every OGRLayer                                     */
    /* --------------------------------------------------------------------- */
    nLayers = pPGDS->GetLayerCount();
    cout<<"Layers Count: "<<nLayers<<endl;
    int iLyr = 0;
    char* pszSRSWkt = NULL;
    OGRLayer *poLayer = NULL;
    OGRFeature *pTmpFeature = NULL;
    for(; iLyr<nLayers; ++iLyr)
    {
        char* pszSQL = (char*)malloc(sizeof(char)*1024*20);
        sprintf(pszSQL,"SELECT * FROM %s WHERE 1=1", pPGDS->GetLayer(iLyr)->GetName());
        /* sprintf(pszSQL,"SELECT * FROM osm_waterareas"); */
        
        cout<<"Execute SQL: "<<pszSQL<<endl;

        pPGLayer = pPGDS->ExecuteSQL(pszSQL, NULL, NULL);

        char* pszNewName = (char*)malloc(sizeof(char)*1204);
        memset(pszNewName,0,1024);
        strcat(pszNewName, pPGDS->GetLayer(iLyr)->GetName());
        strcat(pszNewName, "_blured_leap_5");

#if defined(DUMPTOPG)
        poLayer = poODS->CreateLayer( pszNewName, pPGLayer->GetSpatialRef() );
#else
        poODS = pDriver->CreateDataSource(pszNewName);
        poLayer = poODS->CreateLayer(pszNewName, pPGLayer->GetSpatialRef());
#endif

        cout<<"Layer "<<iLyr<<": "<<pPGDS->GetLayer(iLyr)->GetName()<<endl;
    /* --------------------------------------------------------------------- */
    /*     Do reduce points                                                  */
    /* --------------------------------------------------------------------- */
        int nFeature = pPGLayer->GetFeatureCount();
        cout<<"feature Count: "<<nFeature<<endl;
        int nGeom = pPGLayer->GetLayerDefn()->GetGeomFieldCount();
        int nAttr = pPGLayer->GetLayerDefn()->GetFieldCount();

        /* print out attributes */
        int iFld;
        cout<<"Field Count: "<<nAttr<<endl;
        for(iFld=0; iFld<nAttr; ++iFld)
        {
            cout<<"Field "<<iFld<<": "<<
                pPGLayer->GetLayerDefn()->GetFieldDefn(iFld)->GetNameRef()<<endl;
        }

        /* Extract Geometry Points */
        int iFea = 0;
        for(; iFea< nFeature; ++iFea)
        {
            DEBUG;
            pTmpFeature = pPGLayer->GetNextFeature();
            
            if( !pTmpFeature )
                break;

            cout<<"handling layerName:          "<<pszNewName<<endl;
            cout<<"handling feature:          "<<iFea<<endl;
            cout<<"Execute SQL: "<<pszSQL<<endl;

            OGRGeometry* pTmpGeom = NULL;
            OGRGeometry* pBluredGeom = NULL;
            int iGeom = 0;
            OGRwkbGeometryType eGeomType;
            for( ; iGeom < nGeom; ++iGeom )
            {
                pTmpGeom = pTmpFeature->GetGeomFieldRef(iGeom);
                eGeomType = pTmpGeom->getGeometryType();

                /* Do blur geometry */
                pBluredGeom = blurGeom( pTmpGeom );

                pTmpFeature->SetGeomFieldDirectly( iGeom, pBluredGeom );
            }

            poLayer->CreateFeature(pTmpFeature);
            OGRFeature::DestroyFeature(pTmpFeature);
            pTmpFeature = NULL;
        }

        free(pszSQL);
        free(pszNewName);
#ifndef DUMPTOPG
        OGRDataSource::DestroyDataSource(poODS);
        poODS = NULL;
#endif

    }
#if defined(DUMPTOPG)
    OGRDataSource::DestroyDataSource(poODS);
#endif

    OGRDataSource::DestroyDataSource(pPGDS);
    pPGDS = NULL;

    return 0;
}

