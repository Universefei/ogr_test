#include "ogrsf_frmts.h"

int main()

{
    /// Register all OGR drivers
    OGRRegisterAll();
    printf("register complete\n");

    const char *pszDriverName;
    OGRSFDriver *poDriver;

    /// Create and Open a PostgreSQL DataSource.
    pszDriverName = "PostgreSQL";
    poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName);
    const char *pszConnInfo = "PG: host='192.168.1.99' port='5432' user='postgres' password='postgres' dbname='opengeo'";
    OGRDataSource *poPgDS = poDriver->Open(pszConnInfo);
    /// Create and Open a GeoJSON DataSource.
    pszDriverName = "GeoJSON";
    poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName);
    const char *pszOutFileName = "outGeojson";
    OGRDataSource *poGeojsonDS = poDriver->CreateDataSource(pszOutFileName);

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
                                  FROM waterply_900913 \
                                  WHERE ST_Contains( ST_MakeEnvelope(-8584936,4691869,-8561767,4710000),geom)";
    //const char *pszSQLCommand = "SELECT ST_AsText(geom) As fei FROM waterply_900913 where gid=3";
    poPgLayer = poPgDS->ExecuteSQL(pszSQLCommand, NULL, NULL);

    /// Translate PgLayer to geoJsonLayer
    /// copyLayer will call CreateLayer which will add new Layer into datasource Layer list.
    /// Why not new a Layer object, but using CreateLayer function? abstract and concreate!
    OGRLayer    *poGeojsonLayer = poGeojsonDS->CopyLayer(poPgLayer, "waterply_900913");

    // ReleaseDataSource, if not invoke this function, generated gml or geojson may
    // \be not complete.
    OGRSFDriverRegistrar::GetRegistrar()->ReleaseDataSource(poGeojsonDS);
    return 0;
}
