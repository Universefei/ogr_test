#ifndef DATAOPERATION_H
#define DATAOPERATION_H

#include <iostream>
//#include "OWSService.h"
#include <vector>
#include <json.h>
#include <cpl_vsi.h>
#include <ogrsf_frmts.h>


typedef std::vector<std::string> charStream;

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
/*                              DataOperation                               */
/****************************************************************************/
class DataOperation
{
public:
    DataOperation();
    ~DataOperation();

    //
    // WFS Operation
    //
    static FileInfo         genFeatureFile(const std::string& layerName, 
                                            const BBOX& boundingbox,
                                            const charStream& attrColumn);
    static FileInfo         describeFeatureType(const std::string &);
    static FileInfo         getCapabilities();

    int                     setGenFileFormat(const char* string);

    //
    // Serialization
    //
    static void             SerializeToDiskGeoJSON(OGRLayer* pSrcLayer, 
                                                    const char* fileName);
    static void             SerializeToMemGeoJSON(OGRLayer* pSrcLayer, 
                                                    void*   pLocation,
                                                    int&    rLength);

//Private:
    //static CPLString genFileFormat_;
};


class PGFilter
{
    char*                   sqlStatement_;
    struct BBOx*                   boundingBox_;
    char*                   layerName_;
public:
    char*                   toSQLStatement();

}

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
    list<char*>*            rsltQue_;

    mutex*                  queMutex_;

public:
    PGAccessor();
    ~PGAccessor();

    OGRLayer*               ExecuteSQL();
    OGRLayer*               ExecuteSQL(const char** sqlStatement);

    OGRDataSource*          ConnDB( char* host,
                                    char* port,
                                    char* user,
                                    char* password,
                                    char* dbname );

    //
    // Output 
    //
    void                    ShowRsltOnTerm();
    void*                   DumpRsltToJsonOnDisk(const char* filename);
    list<char*>*            DumpRsltToJsonOnMemQue();
    void*                   ConvertRsltToObj();



    //
    // WFS Operation
    //
    static FileInfo         genFeatureFile(const std::string& layerName, 
                                            const BBOX& boundingbox,
                                            const charStream& attrColumn);


Private:
    //static CPLString genFileFormat_;
    void                    GetJsonHeader();
    void                    GetJsonFeaturePage();
    void                    GetJsonTail();

};
#endif  // DATAOPERATION_H
