/******************************************************************************
 * $Id: ogrmemdatasource.cpp 26302 2013-08-11 16:24:14Z rouault $
 * * Project:  OpenGIS Simple Features Reference Implementation
 * Purpose:  Implements OGRVTDataSource class.
 * Author:   Frank Warmerdam, warmerdam@pobox.com
 *
 ****************************************************************************/

#include "ogr_vt.h"
#include "cpl_conv.h"
#include "cpl_string.h"
#include "ogr_spatialref.h"
#include "ogrvtlayer.h"
#include "metadata.h"
#include <vector>
#include <string>
#include <stdio.h>
#include <string.h>
using namespace std;


CPL_CVSID("$Id: ogrvtdatasource.cpp 26302 2013-08-11 16:24:14Z rouault $");

/************************************************************************/
/*                          OGRVTDataSource()                          */
/************************************************************************/

OGRVTDataSource::OGRVTDataSource( const char *pszFilename, 
                                    char **papszOptions)

{
    pszName = CPLStrdup(pszFilename);
    papoLayers = NULL;
    nLayers = 0;
    dataDir = new char[500];
}

OGRVTDataSource::OGRVTDataSource()

{
    pszName = NULL;
    papoLayers = NULL;
    nLayers = 0;
    dataDir = new char[500];
}

/************************************************************************/
/*                         ~OGRVTDataSource()                          */
/************************************************************************/

OGRVTDataSource::~OGRVTDataSource()

{
    CPLFree( pszName );

    for( int i = 0; i < nLayers; i++ )
         free(papoLayers[i]);
    
    CPLFree( papoLayers );
    delete[] dataDir;
}

/************************************************************************/
/*                             Open()                                  */
/************************************************************************/

int OGRVTDataSource::Open( const char * pszNewName, int bUpdate,
                              int bTestOpen )

{
    int bRet = 0;
    CPLAssert( nLayers == 0 );
    /**
     * eample conninfo:
     * "VT: dir=/home/gdal/vt" 
     */
    
/* -------------------------------------------------------------------- */
/*      Verify vector tile prefix.                                      */
/* -------------------------------------------------------------------- */
    if( !EQUALN(pszNewName,"VT:",3) )
    {
        if( !bTestOpen )
            CPLError( CE_Failure, CPLE_AppDefined,
                      "%s does not conform to Vector Tile naming convention,"
                      " VT:*\n", pszNewName );
        return FALSE;
    }

    pszName = CPLStrdup( pszNewName );
    char* pszConnectionName = CPLStrdup(pszName);

/* -------------------------------------------------------------------- */
/*      Parse out kv_store                                              */
/* -------------------------------------------------------------------- */
    char             *pszKvStoreStart;
    CPLString         osKvStore;

    pszKvStoreStart = strstr(pszConnectionName, "dir=");
    if (pszKvStoreStart == NULL)
        pszKvStoreStart = strstr(pszConnectionName, "dir=");
    if (pszKvStoreStart != NULL)
    {
        char           *pszKvStore;
        const char     *pszEnd = NULL;

        pszKvStore = CPLStrdup( pszKvStoreStart + strlen("dir=") );

        pszEnd = strchr(pszKvStoreStart, ' ');
        if( pszEnd == NULL )
            pszEnd = pszConnectionName + strlen(pszConnectionName);

        // Remove KV_STORE=xxxxx from pszConnectionName string
        memmove( pszKvStoreStart, pszEnd, strlen(pszEnd) + 1 );

        pszKvStore[pszEnd - pszKvStoreStart - strlen("dir=")] = '\0';

        osKvStore = pszKvStore;
        strcpy(dataDir, pszKvStore );
        printf("dataDir = %s\n", dataDir);

        CPLFree(pszKvStore);
    }


/* -------------------------------------------------------------------- */
/*      Try to get layers list                                          */
/* -------------------------------------------------------------------- */
    /* char layerListPath[500]; */
    char* layerListPath = (char*)malloc(500);
    strcat(layerListPath, dataDir);
    strcat(layerListPath, "/metadata.json");
    printf("metadata.json path = %s\n", layerListPath);

    vector<string> layerList = *(getLayerList(layerListPath));


/* -------------------------------------------------------------------- */
/*      create layer via layer name                                     */
/* -------------------------------------------------------------------- */

    vector<string>::iterator itr;
    /* char* layerMetaPath = new char[500]; */
    char* layerMetaPath = (char*)malloc(500);
    for( itr=layerList.begin(); itr!=layerList.end(); itr++)
    {
        /* char layerMetaPath[500]; */
        if(layerMetaPath) memset(layerMetaPath, 0, 500);
        strcat(layerMetaPath, dataDir);
        strcat(layerMetaPath, "/");
        strcat(layerMetaPath, itr->c_str());
        strcat(layerMetaPath, "_meta.json");
        printf("layer_meta.json path = %s\n", layerMetaPath);

        OGRVTLayer* poLayer = generateLayer(layerMetaPath);
        poLayer->setDataSource(this);

        if(poLayer)
        {
            printf("==========================================\n");
            printf("parsed out layer is : %s\n", poLayer->GetName());
            OGRFeatureDefn* poLayerDefn = poLayer->GetLayerDefn();
            int nField = poLayerDefn->GetFieldCount();
            for(int i = 0; i < nField; ++i)
            {
                OGRFieldDefn* poField = poLayerDefn->GetFieldDefn(i);
                printf(" field name is : %s\n", poField->GetNameRef());
                printf(" field type is : %d\n", poField->GetType());
            }
        }
        else
        {
            fprintf(stderr, "parse layer error\n");
        }

        papoLayers = (OGRVTLayer **)
            CPLRealloc( papoLayers,  sizeof(OGRVTLayer *) * (nLayers+1) );
        
        printf("wtf\n");
        papoLayers[nLayers++] = poLayer;
    }
    if(NULL != layerMetaPath)
    {
        delete layerMetaPath;
        printf("delete error?\n");
    }

    return 1;
}

/************************************************************************/
/*                            CreateLayer()                             */
/************************************************************************/

OGRLayer *
OGRVTDataSource::CreateLayer( const char * pszLayerName,
                                 OGRSpatialReference *poSRS,
                                 OGRwkbGeometryType eType,
                                 char ** papszOptions )

{
/* -------------------------------------------------------------------- */
/*      Create the layer object.                                        */
/* -------------------------------------------------------------------- */
    OGRVTLayer *poLayer;

    poLayer = new OGRVTLayer( pszLayerName, poSRS, eType );

/* -------------------------------------------------------------------- */
/*      Add layer to data source layer list.                            */
/* -------------------------------------------------------------------- */
    papoLayers = (OGRVTLayer **)
        CPLRealloc( papoLayers,  sizeof(OGRVTLayer *) * (nLayers+1) );
    
    papoLayers[nLayers++] = poLayer;

    return poLayer;
}

/************************************************************************/
/*                            DeleteLayer()                             */
/************************************************************************/

OGRErr OGRVTDataSource::DeleteLayer( int iLayer )

{
    if( iLayer >= 0 && iLayer < nLayers )
    {
        delete papoLayers[iLayer];

        for( int i = iLayer+1; i < nLayers; i++ )
            papoLayers[i-1] = papoLayers[i];
        
        nLayers--;
        
        return OGRERR_NONE;
    }
    else
        return OGRERR_FAILURE;
}

/************************************************************************/
/*                           TestCapability()                           */
/************************************************************************/

int OGRVTDataSource::TestCapability( const char * pszCap )

{
    if( EQUAL(pszCap,ODsCCreateLayer) )
        return TRUE;
    else if( EQUAL(pszCap,ODsCDeleteLayer) )
        return TRUE;
    else if( EQUAL(pszCap,ODsCCreateGeomFieldAfterCreateLayer) )
        return TRUE;
    else
        return FALSE;
}

/************************************************************************/
/*                              GetLayer()                              */
/************************************************************************/

OGRLayer *OGRVTDataSource::GetLayer( int iLayer )

{
    if( iLayer < 0 || iLayer >= nLayers )
        return NULL;
    else
        return papoLayers[iLayer];
}

