#include <ogrsf_frmts.h>
#include <ogr_core.h> /* OGRwktGeomType */
#include <iostream>
using namespace std;

/******************************************************************************
 *                          blurLineString                                    *
 *****************************************************************************/

OGRLineString* blurLineString(OGRLineString *pGeom)
{
    /**
     * do not new OGRLineString directly
     * because, feature delete it using gdal dynamic lib run time.
     */

    /* verbose info */
    cout<<"Before blurLineString:"<<endl;
    char* pszWktGeom = NULL;
    pGeom->exportToWkt(&pszWktGeom);
    cout<<pszWktGeom<<endl;
    CPLFree(pszWktGeom);
    pszWktGeom = NULL;

    OGRGeometry* pNewGeom = OGRGeometryFactory::createGeometry( wkbLineString );
    OGRLineString* pRetGeom = dynamic_cast< OGRLineString* >(pNewGeom);

    OGRSpatialReference* pSRS = pGeom->getSpatialReference();
    pRetGeom->assignSpatialReference(pSRS);

    int pointCnt = pGeom->getNumPoints();
    OGRPoint* pTmpPoint = dynamic_cast< OGRPoint* >(
            OGRGeometryFactory::createGeometry(wkbPoint) );
    for(int i=0; i<pointCnt; i+=9)
    {
        pGeom->getPoint( i, pTmpPoint );
        pRetGeom->addPoint( pTmpPoint );
    }

    OGRPoint* pLastPoint = dynamic_cast< OGRPoint* >(
            OGRGeometryFactory::createGeometry(wkbPoint) );
    pGeom->getPoint( pointCnt-1, pLastPoint );
    if( pLastPoint == pTmpPoint )
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

    OGRSpatialReference* pSRS = pGeom->getSpatialReference();
    pRetGeom->assignSpatialReference(pSRS);

    // exterior Ring of Polygon
    OGRLineString* pExtRing = pGeom->getExteriorRing();
    OGRLineString* pBluredExtRing = blurLineString(pExtRing);
    OGRLinearRing* pLinearRing = dynamic_cast< OGRLinearRing* >(pBluredExtRing);

    pRetGeom->addRing( pLinearRing );

    // interior Rings of Polygon
    int iRing = 0;
    int nRingCnt = pGeom->getNumInteriorRings();
    for( ; iRing < nRingCnt; ++iRing)
    {
        OGRLineString* pInRing = pGeom->getInteriorRing(iRing);
        OGRLineString* pBluredInRing = blurLineString(pInRing);
        OGRLinearRing* pLinearRing = dynamic_cast< OGRLinearRing* >(pBluredInRing);

        pRetGeom->addRing( pLinearRing );
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
                cout<<"Point"<<endl;
                pNewGeom = OGRGeometryFactory::createGeometry(wkbPoint);
                OGRPoint* pRetGeom = dynamic_cast< OGRPoint* >(pNewGeom);
                pNewGeom->assignSpatialReference(pSRS);

				OGRPoint* pPoint = dynamic_cast< OGRPoint* >(pGeom);
				if(!pPoint) break;

                pRetGeom->setX( pPoint->getX() );
                pRetGeom->setY( pPoint->getY() );
                pRetGeom->setZ( pPoint->getZ() );
				//break;
                cout<<"before blurGeom return"<<endl;
                return pRetGeom;
			}

        case wkbLineString:
			{
                cout<<"LintString"<<endl;
				OGRLineString* pLString = dynamic_cast<OGRLineString*>(pGeom);
				if(!pLString) break;

                OGRLineString* pRetGeom = blurLineString(pLString);
                cout<<"blurGeom for LineString            OK"<<endl;
                //break;
                cout<<"before blurGeom return"<<endl;
                return pRetGeom;
			}

        case wkbPolygon:
			{
                cout<<"Polygon"<<endl;
				OGRPolygon* pPolygon = dynamic_cast<OGRPolygon*>(pGeom);
				if(!pPolygon) break;

                OGRPolygon* pRetGeom = blurPolygon(pPolygon);
                cout<<"blurGeom for Polygon               OK"<<endl;

                //break;
                cout<<"before blurGeom return"<<endl;
                return pRetGeom;
			}

		case wkbMultiLineString:
			{
                cout<<"MultiLineString"<<endl;
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
                cout<<"blurGeom for MultiLineString       OK"<<endl;

				//break;
                cout<<"before blurGeom return"<<endl;
                return pRetGeom;
			}

		case wkbMultiPolygon:
			{
                cout<<"MultiPolygon"<<endl;
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
                cout<<"blurGeom for MultiPolygon          OK"<<endl;

				//break;
                cout<<"before blurGeom return"<<endl;
                return pRetGeom;
			}
        default: 
            //break;
            cout<<"Not recognised"<<endl;
            cout<<"before blurGeom return"<<endl;
            return NULL;
    }
}


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
            cout<<"handling feature:          "<<iFea<<endl;

            OGRGeometry* pTmpGeom = NULL;
            OGRGeometry* pBluredGeom = NULL;
            int nGeom = pPGLayer->GetLayerDefn()->GetGeomFieldCount();
            int iGeom = 0;
            OGRwkbGeometryType eGeomType;
            for( ; iGeom < nGeom; ++iGeom )
            {
                pTmpGeom = pTmpFeature->GetGeomFieldRef(iGeom);
                eGeomType = pTmpGeom->getGeometryType();

                /* Do blur geometry */
                cout<<"start calling blurGeom                 --"<<endl;
                pBluredGeom = blurGeom( pTmpGeom );
                cout<<"finish calling blurGeom                 --"<<endl;

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

