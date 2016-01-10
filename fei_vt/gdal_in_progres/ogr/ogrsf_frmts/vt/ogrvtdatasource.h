#ifndef __OGRVTDATASOURCE_H__
#define __OGRVTDATASOURCE_H__

#include "ogrsf_frmts.h"

class OGRVTLayer;

class OGRVTDataSource : public OGRDataSource
{
    OGRVTLayer          **papoLayers;
    int                 nLayers;
    char                *pszName;
    char*               dataDir;


  public:

                        OGRVTDataSource( const char *, char ** );
                        OGRVTDataSource();
                        ~OGRVTDataSource();

    int                 Open( const char *, int bUpdate, int bTestOpen ); /* fei */

    const char          *GetName() { return pszName; }
    int                 GetLayerCount() { return nLayers; }
    OGRLayer            *GetLayer( int );
    const char*         getDataDir(){ return dataDir;};

    virtual OGRLayer    *CreateLayer( const char *, 
                                      OGRSpatialReference * = NULL,
                                      OGRwkbGeometryType = wkbUnknown,
                                      char ** = NULL );
    OGRErr              DeleteLayer( int iLayer );

    int                 TestCapability( const char * );
};

#endif
