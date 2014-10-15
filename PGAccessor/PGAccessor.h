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
/**
 * adopter design pattern & composite
 */
class PGAccessor
{
    OGRDataSource*          pgDS_;

    //PGFilter*               filter_;
    char*                   sqlStatement_;
    OGRLayer*               pRsltLayer_;

    //
    // Output medium
    void*                   thirdObj_;
    BufList*                rsltQue_;

    pthread_mutex_t         mutex_;

public:
    PGAccessor();
    ~PGAccessor();

    //
    // geter and setters
    void                    SetSQL(char*);
    void                    SetSQL(const string& layerName,
                                        const BBOX& boundingBox,
                                        const fieldVec& attrColumn);
    BufList*                GetRsltList();
    bool                    IsQueEmpty();
    int                     GetNextJsonSeg(pair<char*,int>**);

    
    //
    // DB and connection
    OGRLayer*               ExecuteSQL();
    OGRDataSource*          ConnDB( const char* host,
                                    const char* port,
                                    const char* user,
                                    const char* password,
                                    const char* dbname );

    //
    // Database and Table Metadata accessors
    vector<string>          getAllTablesNames();
    int                     getTableCnt();
    OGRFeatureDefn*         getTableFeatureDefn(string tableName) const;
    OGRSpatialReference*    getTableSpatialRef(string tableName) const;

    int                     getTableGeomFieldsCnt(string tableName) const;
    vector<string>          getTableGeomFieldsNames(string tableName) const;
    string                  getTableGeomFieldName(int = 0) const;
    OGRGeomeDefn*           getTableGeomFieldDefn(int = 0) const;
    OGRGeomeDefn*           getTableGeomFieldDefn(string geomName) const;

    int                     getTableGeomFieldsCnt(string tableName) const;
    vector<string>          getTableAttrFieldsNames(String tableName) const;
    string                  getTableAttrFieldName(int = 0) const;
    OGRFieldDenf*           getTableAttrFieldDefn(int = 0) const;
    OGRFieldDenf*           getTableAttrFieldDefn( stirng AttrName) const;

    // 

    //
    // Output 
    void                    ShowRsltOnTerm();
    const char*             DumpRsltToJsonOnDisk(const char* filename);
    BufList*                DumpRsltToJsonOnMemQue();
    void*                   ConvertRsltToObj();

private:
    void                    GetJsonHeader(char** outFlow, int& retLen);
    int                     GetJsonFeaturePage(char** outFlow, int& retLen);

};


#endif  // PGACCESSOR_H
