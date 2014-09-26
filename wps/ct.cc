#include <ogrsf_frmts.h>
#include "ogr_spatialref.h"
#include "wps_ct.h"
#include <iostream>
using namespace std;

/******************************************************************************
 *                            transfomate()                                   *
 *****************************************************************************/

int transformate( const std::string &original_coordinate, 
                  const std::string &target_coordinate, 
                  const std::string &original_file, 
                  const std::string &target_file)
{
    const char* pszDataSource = original_file.c_str();
    const char* pszDestDataSource = target_file.c_str();
    const char* pszOutputSRSDef = target_coordinate.c_str();
    const char* pszSourceSRSDef = original_coordinate.c_str();

    OGRSpatialReference *poOutputSRS = NULL;
    OGRSpatialReference *poSourceSRS = NULL;

    OGRDataSource       *poDS = NULL;
    OGRDataSource       *poODS = NULL;
    OGRSFDriver         *poDriver = NULL;
    int                  bCloseODS = TRUE;

/* -------------------------------------------------------------------- */
/*      Open the input data source.                                     */
/* -------------------------------------------------------------------- */
    poDS = OGRSFDriverRegistrar::Open( pszDataSource, FALSE, &poDriver );
    if( poDS == NULL )
    {
        OGRSFDriverRegistrar    *poR = OGRSFDriverRegistrar::GetRegistrar();
        
        fprintf( stderr, "WPS_CT:\n"
                "Unable to open datasource `%s' with the following drivers.\n",
                pszDataSource );

        for( int iDriver = 0; iDriver < poR->GetDriverCount(); iDriver++ )
        {
            fprintf( stderr, "  -> %s\n", poR->GetDriver(iDriver)->GetName() );
        }

        exit( 1 );
    }

/* -------------------------------------------------------------------- */
/*      Create the output data source.                                  */
/* -------------------------------------------------------------------- */
    poODS = poDriver->CreateDataSource( pszDestDataSource );
    if( poODS == NULL )
    {
        fprintf( stderr,  "WPS_CT:%s driver failed to create %s\n", 
                poDriver->GetName(), pszDestDataSource );
        exit( 1 );
    }

/* -------------------------------------------------------------------- */
/*      Parse the output SRS definition                                 */
/* -------------------------------------------------------------------- */
    if( pszOutputSRSDef != NULL )
    {
        poOutputSRS = (OGRSpatialReference*)OSRNewSpatialReference(NULL);
        if( poOutputSRS->SetFromUserInput( pszOutputSRSDef ) != OGRERR_NONE )
        {
            fprintf( stderr,  "WPS_CT:Failed to process SRS definition: %s\n", 
                    pszOutputSRSDef );
            exit( 1 );
        }
    }

/* -------------------------------------------------------------------- */
/*      Parse the source SRS definition                                 */
/* -------------------------------------------------------------------- */
    if( pszSourceSRSDef != NULL )
    {
        poSourceSRS = (OGRSpatialReference*)OSRNewSpatialReference(NULL);
        if( poSourceSRS->SetFromUserInput( pszSourceSRSDef ) != OGRERR_NONE )
        {
            fprintf( stderr,  "WPS_CT:\nFailed to process SRS definition: %s\n", 
                    pszSourceSRSDef );
            exit( 1 );
        }
    }
 
/* -------------------------------------------------------------------- */
/*      Create a transformation object from the source to               */
/*      destination coordinate system.                                  */
/* -------------------------------------------------------------------- */
    OGRCoordinateTransformation* poUserCT = OGRCreateCoordinateTransformation(
            poSourceSRS, poOutputSRS);
    if( NULL == poUserCT )
    {
        fprintf( stderr, "WPS_CT: create OGRCreateCoordinateTransformation error\n");
        exit(1);
    }
    OGRCoordinateTransformation* poCT = NULL;

/* -------------------------------------------------------------------- */
/*      Process all data source layer.                                  */
/* -------------------------------------------------------------------- */
    int             nLayerCount = poDS->GetLayerCount();
    OGRLayer**      papoLayers =  
                    (OGRLayer**)CPLMalloc(sizeof(OGRLayer*) * nLayerCount);

    for( int iLayer = 0; 
         iLayer < nLayerCount; 
         iLayer++ )
    {
        OGRLayer        *poLayer = poDS->GetLayer(iLayer);
        if( poLayer == NULL )
        {
            fprintf( stderr, "WPS_CT: Couldn't fetch advertised layer %d!\n",
                    iLayer );
            exit( 1 );
        }
        papoLayers[iLayer] = poLayer;
    }
/* -------------------------------------------------------------------- */
/*      Process specified data source layers.                           */
/* -------------------------------------------------------------------- */
    int nRetCode = 0;
    for( int iLayer = 0; 
        iLayer < nLayerCount && nRetCode == 0; 
        iLayer++ )
    {
        OGRLayer        *poLayer = papoLayers[iLayer];
        if (poLayer == NULL)
            continue;

       /** 
        * if can parse input datasource layer spatial reference,
        * ignore user input source spatial reference infomation
        */ 
        OGRSpatialReference     *poRealInSRS = poLayer->GetSpatialRef();
        if( NULL != poRealInSRS)
        {
            poCT = OGRCreateCoordinateTransformation( 
                    poRealInSRS, poOutputSRS);
        }
        poCT = poRealInSRS && poCT ? poCT:
            poUserCT? poUserCT:NULL;

        /* do the real CT */
        if( TransformLayer(poLayer, 0, poCT) != 1 )
        {
            CPLError( CE_Failure, CPLE_AppDefined, 
                    "Terminating translation prematurely after failed\n"
                    "translation of layer %s (use -skipfailures to skip errors)\n", 
                    poLayer->GetName() );
            nRetCode = 1;
        }

        poODS->CopyLayer(poLayer, poLayer->GetName(), NULL);
    }

    CPLFree(papoLayers);
    OGRDataSource::DestroyDataSource(poDS);
    OGRDataSource::DestroyDataSource(poODS);
    return 0;
};


/******************************************************************************
 *                        TransformLayer()                                    *
 *****************************************************************************/

int TransformLayer( OGRLayer* poSrcLayer,
                          int iGeomField ,
                          OGRCoordinateTransformation* poCT )
{
    if(NULL == poCT)
        return 1;
    if(poSrcLayer->GetLayerDefn()->GetGeomFieldCount() <= iGeomField )
        return 1;

    int             feaCount = poSrcLayer->GetFeatureCount();
    OGRFeature*     poFeature = NULL;
    OGRGeometry*    poDstGeom = NULL;
    OGRGeometry*    poReprojectedGeom = NULL;
    for(int i=0; i<feaCount; ++i)
    {
        poFeature = poSrcLayer->GetNextFeature();
        if(poFeature)
        {
            poDstGeom = poFeature->GetGeomFieldRef(iGeomField);
            poReprojectedGeom =
                OGRGeometryFactory::transformWithOptions(poDstGeom, poCT, NULL);
            poFeature->SetGeomFieldDirectly(iGeomField, poReprojectedGeom);
            // TODO: to destroy poDstGeom
            poDstGeom = poReprojectedGeom;
        }
        else
            break;
    }
    return 1;
};
