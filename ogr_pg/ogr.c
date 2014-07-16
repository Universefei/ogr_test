#include "gdal/gdal.h"

int main()

{
    GDALAllRegister();

    GDALDatasetH hDS;
    OGRLayerH hLayer;
    OGRFeatureH hFeature;

    hDS = GDALOpenEx( "point.shp", GDAL_OF_VECTOR, NULL, NULL, NULL );
    if( hDS == NULL )
    {
        printf( "Open failed.\n" );
        exit( 1 );
    }

    hLayer = GDALDatasetGetLayerByName( hDS, "point" );

    OGR_L_ResetReading(hLayer);
    while( (hFeature = OGR_L_GetNextFeature(hLayer)) != NULL )
    {
        OGRFeatureDefnH hFDefn;
        int iField;
        OGRGeometryH hGeometry;

        hFDefn = OGR_L_GetLayerDefn(hLayer);

        for( iField = 0; iField < OGR_FD_GetFieldCount(hFDefn); iField++ )
        {
            OGRFieldDefnH hFieldDefn = OGR_FD_GetFieldDefn( hFDefn, iField );

            if( OGR_Fld_GetType(hFieldDefn) == OFTInteger )
                printf( "%d,", OGR_F_GetFieldAsInteger( hFeature, iField ) );
            else if( OGR_Fld_GetType(hFieldDefn) == OFTReal )
                printf( "%.3f,", OGR_F_GetFieldAsDouble( hFeature, iField) );
            else if( OGR_Fld_GetType(hFieldDefn) == OFTString )
                printf( "%s,", OGR_F_GetFieldAsString( hFeature, iField) );
            else
                printf( "%s,", OGR_F_GetFieldAsString( hFeature, iField) );
        }

        hGeometry = OGR_F_GetGeometryRef(hFeature);
        if( hGeometry != NULL 
                && wkbFlatten(OGR_G_GetGeometryType(hGeometry)) == wkbPoint )
        {
            printf( "%.3f,%3.f\n", OGR_G_GetX(hGeometry, 0), OGR_G_GetY(hGeometry, 0) );
        }
        else
        {
            printf( "no point geometry\n" );
        }

        OGR_F_Destroy( hFeature );
    }

    GDALClose( hDS );
}
