#ifndef PGACCESSOR_H
#define PGACCESSOR_H

#include <iostream>
#include <vector>
#include <list>
#include <pthread.h>

#include <json.h>

#include <cpl_vsi.h>
#include <ogrsf_frmts.h>

using namespace std;


typedef vector<string> fieldVec;
typedef list< pair< char*,int>* > BufList;

struct BBOX
{
    double xmin;
    double ymin;
    double xmax;
    double ymax;
};


/****************************************************************************/
/*                          class  PGAccessor                               */
/****************************************************************************/
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
    //
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
    // Output 
    //
    void                    ShowRsltOnTerm();
    const char*             DumpRsltToJsonOnDisk(const char* filename);
    BufList*                DumpRsltToJsonOnMemQue();
    void*                   ConvertRsltToObj();

private:
    void                    GetJsonHeader(char** outFlow, int& retLen);
    int                     GetJsonFeaturePage(char** outFlow, int& retLen);

};
#endif  // PGACCESSOR_H
