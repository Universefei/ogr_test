// pgacc.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

// main.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#include "ogrsf_frmts.h"
//#include "ogr_core.h"
//#include "getPoints.h"

#include <iostream>
using namespace std;

int main(int argc, char* argv[])
{
    /// Register all OGR drivers
    OGRRegisterAll();
    printf("register complete\n");

    /// Get PostgreSQL driver throught Singleton Manager of OGRSFDriverRegistrar.
    const char *pszDriverName = "PostgreSQL";
    OGRSFDriver *poDriver;
    poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName);
    if(NULL == poDriver)
    {
        printf("Can not get driver \n");
    }
    else
    {
        printf("Get Driver succeesfully\n");
    }

    /// Open PostgreSQL database and establish connection.
    OGRDataSource *poDS;
    //const char *pszConnInfo = "PG: host='192.168.1.99' port='5432' user='postgres' password='postgres' dbname='china_osm'";
	const char *pszConnInfo = "PG: host='10.61.124.229' port='5432' user='postgres' password='postgres' dbname='opengeo'";
    poDS = poDriver->Open(pszConnInfo);
    if(NULL == poDS)
    {
        printf("Can not connect to database\n");
    }
    else
    {
        printf("Connection established\n");
    }

    /// Get Layer from DataSource.
    OGRLayer  *poLayer;
    ///  Coder have 3 method in OGRDataSource to generate a OGRLayer.
    /// \===========================================================
    /// [1] GetLayer(int)
    /// [2] GetLayerByName("Layer_name")
    /// [3] ExecuteSQL(const char*, OGRGeometry * pSpatialRef, )
    //
    //poLayer = poDS->GetLayer(int);
    //poLayer = poDS->GetLayerByName( "waterply_900913" );
    //const char *pszSQLCommand = "SELECT ST_AsText(geom) as geomWKT  FROM waterply_900913 WHERE ST_Contains( ST_MakeEnvelope(-8584936,4691869,-8561767,4710000),geom)";
	
	//const char* pszSQLCommand = "SELECT * from planet_osm_polygon where osm_id='26331721'OR osm_id='24426593'";
	//const char* pszSQLCommand = "SELECT * from planet_osm_polygon where osm_id='9999999'";
    //const char* pszSQLCommand = "select * from planet_osm_polygon where st_contains(st_makeenvelope(90.0, 35.0, 100.0, 40.0)::geography::geometry,way)";
	//const char *pszSQLCommand = "SELECT ST_AsText(geom) As fei FROM waterply_900913 where gid=3";
    const char* pszSQLCommand = "select * from osm_places";

	poLayer = poDS->ExecuteSQL(pszSQLCommand, NULL, NULL);

    if(NULL == poLayer)
    {
        printf("Get Result Layers failed\n");
    }
    else
    {
        printf("Get Result Layer succussful\n");
    }

    /// Set Query statement here in OGRResultLayer.

    /// Get Layer metadata.
    OGRFeatureDefn *poFDefn = poLayer->GetLayerDefn();
    if(NULL != poFDefn)
    {
        printf("Get Layer Feature definition successfully\n");
    }

    /// Get Feature from Layer.
    OGRFeature *poFeature;
    poLayer->ResetReading();
    int idxFeature = 0;
    while( (poFeature = poLayer->GetNextFeature()) != NULL )
    {


	/*
		srcCoord* pPoints;
		int numPoint;
		getPoints(poFeature,&numPoint,&pPoints);

		delete[] Points;
		*/


        /// Get Attribute fields.
        int iField;
        int iFieldAmount = poFDefn->GetFieldCount();
		printf("%d th Feature \n", idxFeature);
        for( iField = 0; iField < iFieldAmount; iField++ )
        {
            
            OGRFieldDefn *poFieldDefn = poFDefn->GetFieldDefn( iField );

            if( poFieldDefn->GetType() == OFTInteger )
                printf( "%d,", poFeature->GetFieldAsInteger( iField ) );
            else if( poFieldDefn->GetType() == OFTReal )
                printf( "%.3f,", poFeature->GetFieldAsDouble(iField) );
            else if( poFieldDefn->GetType() == OFTString )
                printf( "%s,", poFeature->GetFieldAsString(iField) );
            else
                printf( "%s,", poFeature->GetFieldAsString(iField) );
        }

        /// Get Geometry of Feature.
        OGRGeometry *poGeometry;
        poGeometry = poFeature->GetGeometryRef();
        if( poGeometry != NULL 
                && wkbFlatten(poGeometry->getGeometryType()) == wkbPoint )
        {
            OGRPoint *poPoint = (OGRPoint *) poGeometry;

            printf( "%.3f,%3.f\n", poPoint->getX(), poPoint->getY() );
        }
        else
        {
            printf( "no point geometry\n" );
        }
        OGRFeature::DestroyFeature( poFeature );

		        idxFeature++;
    }
	OGRSFDriverRegistrar::GetRegistrar()->ReleaseDataSource(poDS);
	return 0;
}
















