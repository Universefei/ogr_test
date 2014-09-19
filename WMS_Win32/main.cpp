// main.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#include "ogrsf_frmts.h"
//#include "ogr_core.h"
#include "getPoints.h"

#include <iostream>
#include <iomanip>
using namespace std;

int main(int argc, char* argv[])
{
	/// Step1: Get Driver 
	/// =================
    OGRRegisterAll();
    const char *pszDriverName = "PostgreSQL";
    OGRSFDriver *poDriver;
    poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName);
	if(NULL == poDriver)
    {
        printf("Can not get driver \n");
		return 0;
    }

	/// Step2: Connect DB and get PG datasource
	/// =======================================
    //const char *pszConnInfo = "PG: host='10.61.124.229' port='5432' user='postgres' password='postgres' dbname='opengeo'";
	const char *pszConnInfo = "PG: host='192.168.1.99' port='5432' user='postgres' password='postgres' dbname='dc'";
    OGRDataSource* poDS = poDriver->Open(pszConnInfo);
    if(NULL == poDS)
    {
        printf("Can not connect to database\n");
		return 0;
    }

    /// Step3: Get Layer through SQL statement.
	/// =======================================
    OGRLayer  *poLayer;
    /// [1] GetLayer(int)
    /// [2] GetLayerByName("Layer_name")
    /// [3] ExecuteSQL(const char*, OGRGeometry * pSpatialRef, )
    //
    //poLayer = poDS->GetLayer(int);
    //poLayer = poDS->GetLayerByName( "waterply_900913" );
    //const char *pszSQLCommand = "SELECT ST_AsText(geom) as geomWKT  FROM waterply_900913 WHERE ST_Contains( ST_MakeEnvelope(-8584936,4691869,-8561767,4710000),geom)";
	//const char* pszSQLCommand = "SELECT * from planet_osm_polygon where osm_id='26331721'OR osm_id='24426593'";
	const char* pszSQLCommand = "SELECT * from osm_waterareas ";
	//const char* pszSQLCommand = "select st_astext(geom) from osm_roads where gid=12";
	//const char* pszSQLCommand = "SELECT * from planet_osm_polygon where osm_id='9999999'";
    //const char* pszSQLCommand = "select * from planet_osm_polygon where st_contains(st_makeenvelope(90.0, 35.0, 100.0, 40.0)::geography::geometry,way)";
	//const char *pszSQLCommand = "SELECT ST_AsText(geom) As fei FROM waterply_900913 where gid=3";
    //const char* pszSQLCommand = "select * from osm_places";

	poLayer = poDS->ExecuteSQL(pszSQLCommand, NULL, NULL);

    if(NULL == poLayer)
    {
        printf("Get Result Layers failed\n");
    }

   
    /// Get Layer metadata.
    OGRFeatureDefn *poFDefn = poLayer->GetLayerDefn();
    if(NULL != poFDefn)
    {
        printf("Get Layer Feature definition successfully\n");
    }

    /// Get Feature from Layer.
	/// =======================
    OGRFeature *poFeature;
    poLayer->ResetReading();
    int idxFeature = 0;
    while( (poFeature = poLayer->GetNextFeature()) != NULL )
    {
		cout<<"------------------feature line------------------"<<endl;

		pointStr feaPoints = getPoints(poFeature);
		for(int i=0; i<feaPoints.numPoints; ++i)
		{
			cout<<"X="<<setprecision(12)<<feaPoints.pCoords[i].x<<" ";
			cout<<"Y="<<setprecision(12)<<feaPoints.pCoords[i].y<<" ";
			cout<<"Z="<<setprecision(12)<<feaPoints.pCoords[i].z<<endl;
		}
		destroyPointsStr(&feaPoints);
	}
	OGRSFDriverRegistrar::GetRegistrar()->ReleaseDataSource(poDS);
	return 0;
}
















