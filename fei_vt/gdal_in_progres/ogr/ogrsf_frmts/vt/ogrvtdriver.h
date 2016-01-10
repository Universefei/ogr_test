#ifndef __OGRVTDRIVER_H__
#define __OGRVTDRIVER_H__

#include "ogrsf_frmts.h"

class OGRVTDriver : public OGRSFDriver
{
  public:
                ~OGRVTDriver();
                
    const char *GetName();
    OGRDataSource *Open( const char *, int );

    virtual OGRDataSource *CreateDataSource( const char *pszName,
                                             char ** = NULL );
    
    int                 TestCapability( const char * );
};

#endif
