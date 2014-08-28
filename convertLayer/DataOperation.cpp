#include "ogrsf_frmts.h"
#include "cpl_string.h"
#include "DataOperation.h"
#include <iostream>
#include <string.h>
#include <sys/stat.h>

//
//  Constructor
DataOperation::DataOperation()
{
}

//
// Destructor
DataOperation::~DataOperation()
{
}

/*****************************************************************************/
/*                          ConvertLayer()                                   */
/*****************************************************************************/

/**
 * @brief:  Dump everty feature from a OGRLayer to a hlsFeatureClass.
 */

int DataOperation::ConvertLayer(OGRLayer* pOGRLayer, IPo_FeatureClass* hlsLayer)
{
    // parameters check
    if(NULL==pOGRLayer || NULL==hlsLayer)
    {
        std::cout<<"not specify input or output Layer"<<std::endl;
    }

/*---------------------------------------------------------------------------*/
//      Dump metadata
/*---------------------------------------------------------------------------*/
    /* Get Feature definition */
    OGRFeatureDefn* pOGRFeatureDefn = pOGRLayer->GetLayerDefn();
    int fieldCount = pOGRFeatureDefn->GetFieldCount();
    int geomCount = pOGRFeatureDefn->GeoGeomFieldCount();
    OGRFieldDefn* pFieldDefn = NULL;
    /* get hlsFeatureClass table field set */
    //IPo_TableFieldSet* pHlsTableFieldSet = hlsLayer->Fields();

    hlsLayer->DeleteFeaturesAll();
    hlsLayer->DeleteFieldsAll();

    /* Dump Attributes fields definitions */
    for(int idx=0; idx<fieldCount; ++idx)
    {
        pFieldDefn = pOGRFeatureDefn->GetFieldDefn(idx);

        char* szname=;
        poenumFieldType=;
        unsigned int uiLength=;
        char* szAliasName=;
        long lPrecision=;
        bool bNullable=;
        char* pzDesc=;

        /* call CPo_FeatureClass.AddField() */
        hlsLayer->AddField(szname,poenumFieldType, uiLength, 
                szAliasName,lPrecision,bNullable, pzDesc);
    }

    /* Dump Geometry fields definitions */
    for(int idx=0; idx<geomCount; ++idx)
    {

    }

/*---------------------------------------------------------------------------*/
//      Dump every feature from OGRlayer to hlsFeatureClass.
/*---------------------------------------------------------------------------*/
    // Get feature amount in source layer
    int fCount = pOGRLayer->GetFeatureCount(); 
    for(int fid=0;fid<fCount;++fid)
    {
        // Create a feature in hls feature class.
        IPo_Feature* pDistFeature=hlsLayer->CreateFeature();
        // Get a feature from OGRPGResultLayer.
        OGRFeature* pSrcFeature=pOGRLayer->GetFeature(fid);
        // Convert feature from OGRFeature to hlsFeature.
        convertFeature(pSrcFeature,pDisFeature);
    }
}

/****************************************************************************/
/*                          ConvertFeature()                                */
/****************************************************************************/

/**
 * @brief:  Convert a OGRPGFeature to a hlsFeature.
 */

DataOperation::ConvertFeature(OGRFeature* pOGRFeature, IPo_Feature* hlsFeature)
{
    // Get Feature definition
    OGRFeatureDefn* pFeatureDefn= pOGRFeature->GetDefnRef();
    // Get Fields definition
    // Get Geometries definition

/*---------------------------------------------------------------------------*/
//      Convert attribuite fields
/*---------------------------------------------------------------------------*/



/*---------------------------------------------------------------------------*/
//      Convert geometry 
/*---------------------------------------------------------------------------*/


}
/****************************************************************************/
/*                        genFeatureFile()                                  */
/****************************************************************************/

FileInfo DataOperation::genFeatureFile(const std::string& layerName, 
        const BBOX& boundingBox, const charStream& attrColumn)
{

    
    OGRSFDriver *poDriver;
    srand(time(0));

    // make all column name in one string
    std::string columnStr = "";
    int i=0;
    while (i<attrColumn.size()-1)
    {
        columnStr+=attrColumn[i];
        columnStr+=",";
        i++;
    }
    columnStr+=attrColumn[i];
    
    std::cout<<columnStr<<std::endl;

    /// Register all OGR drivers
    OGRRegisterAll();
    std::cout<<"register driver over"<<std::endl;

    /// Create and Open a PostgreSQL DataSource.
    const char *pszPGDriverName = "PostgreSQL";
    poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszPGDriverName);
    const char *pszConnInfo = "PG: host='192.168.1.99' port='5432' user='postgres' password='postgres' dbname='opengeo'";
    OGRDataSource *poPgDS = poDriver->Open(pszConnInfo);

    std::cout<<"connect db and open driver over"<<std::endl;
    /// Create and Open a GeoJSON DataSource.
    const char *pszGJDriverName = "GeoJson";
    poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszGJDriverName);
    char pszOutFileName[20];
    char randchar[10];
    for(int i=0; i<9; i++)
    {
        randchar[i]='a'+(rand()%25);
    }
    randchar[10]='\0';
    strcat(pszOutFileName,randchar);
    strcat(pszOutFileName,".GJson");
    //const char *pszOutFileName = "fileName";
    OGRDataSource *poGeojsonDS = poDriver->CreateDataSource(pszOutFileName);


    /// Get Layer from DataSource query with an SQL clause.
    CPLString* queryClause = new CPLString;
    queryClause->Printf("SELECT %s FROM %s WHERE ST_Contains(ST_MakeEnvelope(%f, %f, %f, %f),geom)",
                         columnStr.c_str(), layerName.c_str(), 
                         boundingBox.xmin, boundingBox.ymin, 
                         boundingBox.xmax, boundingBox.ymax);
    std::cout<<queryClause->c_str()<<std::endl;
    OGRLayer  *poPgLayer = poPgDS->ExecuteSQL(queryClause->c_str(), NULL, NULL);
    if(NULL == poPgLayer)
    {
        std::cout<<"executeSQL error"<<std::endl;
    }
    delete(queryClause);

    /// Duplicate retrived Layer to specific output file format.
    OGRLayer    *poGeojsonLayer = poGeojsonDS->CopyLayer(poPgLayer, "query_result");

    // ------------------------------------------------------------------------
    // convert OGRGeoJSONLayer to WMS inteface FeatureClass.

    IP0_FeatureClass* hlsLayer = new FeatureClass;
    convertLayer(poGeojsonLayer, hlsLayer);

    //OGRSFDriverRegistrar::GetRegistrar()->ReleaseDataSource(poGeojsonDS);

    /// Construct return object.
    FileInfo prtFile; 
    prtFile.fileName=(std::string)pszOutFileName;
    struct stat fStat;
    stat(pszOutFileName,&fStat);
    prtFile.size = (double)fStat.st_size;
    prtFile.pos = 0;
    prtFile.fileId = 0;
    
    std::cout<<"fileName:  "<<prtFile.fileName<<std::endl;
    std::cout<<"fileSize:  "<<prtFile.size<<std::endl;

    return prtFile;
}


/****************************************************************************/
/*                      describeFeatureType()                               */
/****************************************************************************/

FileInfo describeFeatureType(const std::string & layer)
{
    FileInfo rtFile;

}

/****************************************************************************/
/*                          getCapabilities()                               */
/****************************************************************************/
FileInfo getCapabilities()
{
    FileInfo rtFile;
}



