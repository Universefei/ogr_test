#include "ogrsf_frmts.h"
#include <stdio.h>
#include <stdlib.h>

int main()

{
    /// Register all OGR drivers
    OGRRegisterAll();
    printf("register complete\n");
    printf("before get driver\n");

    //char *pszDriverName = NULL;
    OGRSFDriver *poDriver = NULL;
    printf("md get driver\n");

    /// Create and Open a PostgreSQL DataSource.
    //pszDriverName = "PostgreSQL";
    poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName("PostgreSQL");
    printf("caoget driver\n");
    if(poDriver == NULL)
    {
        fprintf(stderr, "GeoJSON: Get PG driver error \n");
        exit(1);
    }
    else
    {
    }

    const char* pszConnInfo = "PG: host='192.168.1.99' port='5432' user='postgres' password='postgres' dbname='dc'";
    printf("caoget driver\n");
    OGRDataSource *poPgDS = poDriver->Open(pszConnInfo);
    printf("caoqu driver\n");
    if(poPgDS == NULL)
    {
        fprintf(stderr, "GeoJSON: Open PG database error \n");
        exit(1);
    }
    else
    {
        fprintf(stdout, "GeoJSON: Open PG database OK \n");
    }
    /// Create and Open a GeoJSON DataSource.
    //pszDriverName = "GeoJSON";
    poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName("GeoJSON");
    if(poDriver == NULL)
    {
        fprintf(stderr, "GeoJSON: Get %s driver error \n",
                poDriver->GetName() );
        exit(1);
    }
    else
    {
        fprintf(stdout, "GeoJSON: Get %s driver OK \n",
                poDriver->GetName() );
    }
    const char *pszOutFileName = "output";
    OGRDataSource *poGeojsonDS = poDriver->CreateDataSource(pszOutFileName);
    if(poGeojsonDS == NULL)
    {
        fprintf(stderr, "GeoJSON: Create %s datasource using %s driver error\n",
                pszOutFileName, poDriver->GetName() );
        exit(1);
    }
    else
    {
        fprintf(stdout, "GeoJSON: Create %s datasource using %s driver OK\n",
                pszOutFileName, poDriver->GetName() );
    }

    /// Get Layer from DataSource.
    OGRLayer  *poPgLayer;
    ///  Coder have 3 method in OGRDataSource to generate a OGRLayer.
    /// \===========================================================
    /// [1] GetLayer(int)
    /// [2] GetLayerByName("Layer_name")
    /// [3] ExecuteSQL(const char*, OGRGeometry * pSpatialRef, )
    //
    //poLayer = poDS->GetLayer(int);
    //poLayer = poDS->GetLayerByName( "waterply_900913" );
    const char *pszSQLCommand = "SELECT * \
                                  FROM osm_waterareas ";
                                  //WHERE ST_Contains( ST_MakeEnvelope(-8584936,4691869,-8561767,4710000),geom)";
    //const char *pszSQLCommand = "SELECT ST_AsText(geom) As fei FROM waterply_900913 where gid=3";
    poPgLayer = poPgDS->ExecuteSQL(pszSQLCommand, NULL, NULL);

    /// Translate PgLayer to geoJsonLayer
    /// copyLayer will call CreateLayer which will add new Layer into datasource Layer list.
    /// Why not new a Layer object, but using CreateLayer function? abstract and concreate!
    OGRLayer    *poGeojsonLayer = poGeojsonDS->CopyLayer(poPgLayer, "osm_waterareas");

    // ReleaseDataSource, if not invoke this function, generated gml or geojson may
    // \be not complete.
    OGRSFDriverRegistrar::GetRegistrar()->ReleaseDataSource(poGeojsonDS);
    return 0;
}
