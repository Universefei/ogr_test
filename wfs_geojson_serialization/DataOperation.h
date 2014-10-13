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

#endif  // DATAOPERATION_H
