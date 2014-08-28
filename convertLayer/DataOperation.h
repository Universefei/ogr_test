#ifndef DATAOPERATION_H
#define DATAOPERATION_H

#include <iostream>
//#include "OWSService.h"
#include <vector>

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

    static int              ConvertLayer(OGRLayer* pOGRLayer, 
                                        IPo_FeatureClass* hlsLayer);
    static int              ConvertFeature(OGRFeature* pOGRFeature,
                                        IPo_Feature* hlsFeature);

    static FileInfo         genFeatureFile(const std::string& layerName, 
                                            const BBOX& boundingbox,
                                            const charStream& attrColumn);
    static FileInfo         describeFeatureType(const std::string &);
    static FileInfo         getCapabilities();

    int                     setGenFileFormat(const char* string);

//Private:
    //static CPLString genFileFormat_;
};

#endif  // DATAOPERATION_H
