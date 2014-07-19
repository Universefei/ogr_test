#include "gdal/ogrsf_frmts.h"

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
    if(NULL != poPgDS)
    {
        printf("Open poPgDS successfully\n");
    }

    /// Create and Open a GeoJSON DataSource.
    pszDriverName = "GeoJSON";
    poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName);
    if(NULL != poDriver)
    {
        printf("Get GeoJSON Driver successfully\n");
    }

    const char *pszOutFileName = "outGeojson.geojson";
    OGRDataSource *poGeojsonDS = poDriver->CreateDataSource(pszOutFileName);
    if(NULL != poGeojsonDS)
    {
        printf("Open poGeojsonDS successfully\n");
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
    const char *pszSQLCommand = "SELECT ST_AsText(geom) as geomWKT \
                                  FROM waterply_900913 \
                                  WHERE ST_Contains( ST_MakeEnvelope(-8584936,4691869,-8561767,4710000),geom)";
    //const char *pszSQLCommand = "SELECT ST_AsText(geom) As fei FROM waterply_900913 where gid=3";
    poPgLayer = poPgDS->ExecuteSQL(pszSQLCommand, NULL, NULL);

    /// Translate PgLayer to geoJsonLayer
    /// copyLayer will call CreateLayer which will add new Layer into datasource Layer list.
    /// Why not new a Layer object, but using CreateLayer function? abstract and concreate!
    OGRLayer    *poGeojsonLayer = poGeojsonDS->CopyLayer(poPgLayer, "waterply_900913");

    /// Get Layer metadata.(OGRFeatureDefn)
    OGRFeatureDefn *poFDefn = poPgLayer->GetLayerDefn();

    /// Get Feature from Layer.
    OGRFeature *poFeature;
    poPgLayer->ResetReading();
    int idxFeature = 0;
    while( (poFeature = poPgLayer->GetNextFeature()) != NULL )
    {
        idxFeature++;
        /// Get Attribute fields.
        int iField;
        int iFieldAmount = poFDefn->GetFieldCount();
        for( iField = 0; iField < iFieldAmount; iField++ )
        {
            printf("%d th Feature \n", idxFeature);
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
    }

    //GDALClose( poDS );
}
