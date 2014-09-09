#ifndef PGACCESSOR_H
#define PGACCESSOR_H

#include <iostream>
#include <vector>
#include <list>
#include <pthread.h>

#include <json.h>

#include <cpl_vsi.h>
#include <ogrsf_frmts.h>
#include "misc.h"

using namespace std;



/****************************************************************************/
/*                          class  PGAccessor                               */
/****************************************************************************/
class PGAccessor
{
    OGRDataSource*          pgDS_;

    //PGFilter*               filter_;
    char*                   sqlStatement_;
    OGRLayer*               pRsltLayer_;
    BufList*                rsltQue_;

    pthread_mutex_t         mutex_;

public:
    PGAccessor();
    ~PGAccessor();

    //
    // geter and setters
    //
    void                    SetSQL(char*);
    void                    SetSQL(const string& layerName,
                                        const BBOX& boundingBox,
                                        const fieldVec& attrColumn);
    
    //
    // DB and connection
    //
    OGRDataSource*          ConnDB( const char* host,
                                    const char* port,
                                    const char* user,
                                    const char* password,
                                    const char* dbname );
    OGRLayer*               ExecuteSQL();

    //
    // Output 
    //
    void*                   DumpToHlsLayer();

private:
    bool                    IsQueEmpty();
    BufList*                GetRsltList();

    poenumFieldType         MapFieldType(OGRFieldType&);
    poenumFieldType         mapGeomType(OGRGeomFieldType&);
    int                     ConvertLayer(OGRLayer*,IPo_FeatureClass*);

};
#endif  // PGACCESSOR_H
