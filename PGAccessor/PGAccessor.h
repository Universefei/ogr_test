#ifndef DATAOPERATION_H
#define DATAOPERATION_H

#include <iostream>
#include <vector>
#include <list>

#include <json.h>

#include <cpl_vsi.h>
#include <ogrsf_frmts.h>

using namespace std;


typedef std::vector<std::string> charStream;

typedef std::list< std::pair<const char*, int>* > BufList;

struct FileInfo
{
    std::string fileName;
    int fileId;
    long size;
    long pos;
};

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

//    mutex*                  queMutex_;

public:
    PGAccessor();
    //PGAccessor();

    ~PGAccessor();

    //
    // geter and setters
    //
    void                    SetSQL(char*);
    void                    SetQue(BufList*);
    void                    setMutex();
    
    //
    // DB and connection
    OGRLayer*               ExecuteSQL();
    OGRLayer*               ExecuteSQL(const char* sqlStatement);

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



    static FileInfo         genFeatureFile(const std::string& layerName, 
                                            const BBOX& boundingbox,
                                            const charStream& attrColumn);


private:
    void                    GetJsonHeader(char** outFlow, int* retLen);
    int                     GetJsonFeaturePage(char** outFlow, int* retLen);

};
#endif  // DATAOPERATION_H
