#include "ogrsf_frmts.h"
#include "cpl_string.h"
#include "DataOperation.h"
#include <iostream>
#include <string.h>
#include <sys/stat.h>
#include "ogrgeojsonwriter.h"
#include "ogrgeojsonutils.h"
#include "ogr_spatialref.h"
#include <json.h>


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
    const char *pszConnInfo = "PG: host='192.168.1.99' port='5432' user='postgres' password='postgres' dbname='china_osm'";
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
    //OGRDataSource *poGeojsonDS = poDriver->CreateDataSource(pszOutFileName);

//WHERE ST_Contains(ST_MakeEnvelope(%f, %f, %f, %f),geom)",
    /// Get Layer from DataSource query with an SQL clause.
    CPLString* queryClause = new CPLString;
    queryClause->Printf("SELECT * FROM %s WHERE osm_id=126186309",
                         //columnStr.c_str(), 
                         layerName.c_str() 
                         //boundingBox.xmin, boundingBox.ymin, 
                         //boundingBox.xmax, boundingBox.ymax
                         );
    std::cout<<queryClause->c_str()<<std::endl;

    // Get a OGRPGResultLayer via executing SQL statement
    OGRLayer  *poPgLayer = poPgDS->ExecuteSQL(queryClause->c_str(), NULL, NULL);
    if(NULL == poPgLayer)
    {
        std::cout<<"executeSQL error"<<std::endl;
    }
    delete(queryClause);

    SerializeToDiskGeoJSON(poPgLayer, pszOutFileName);

    OGRSFDriverRegistrar::GetRegistrar()->ReleaseDataSource(poPgDS);

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

/*****************************************************************************/
/*                          SerializeToDiskGeoJSON()                         */
/*****************************************************************************/

//BufInfo SerializeToMemGeoJSON(OGRLayer* pSrcLayer)
void DataOperation::SerializeToDiskGeoJSON(OGRLayer* pSrcLayer,const char* pszFilename)
{
    FILE* fpOut_ = VSIFOpenL( pszFilename, "w");
    ///////////////////////////////from OGRGeoJSONDataSource::CreateLayer()
    /* begin of geojson file */
    VSIFPrintfL( fpOut_, "{\n\"type\": \"FeatureCollection\",\n" );

/* ------------------------------------------------------------------------- */
/*      Serialize metadata                                                   */
/* ------------------------------------------------------------------------- */
    OGRSpatialReference *poSRS = pSrcLayer->GetSpatialRef();
    if (poSRS)
    {
        const char* pszAuthority = poSRS->GetAuthorityName(NULL);
        const char* pszAuthorityCode = poSRS->GetAuthorityCode(NULL);
        if (pszAuthority != NULL && pszAuthorityCode != NULL &&
                strcmp(pszAuthority, "EPSG") == 0)
        {

            json_object* poObjCRS = json_object_new_object();
            json_object_object_add(poObjCRS, "type",
                    json_object_new_string("name"));

            json_object* poObjProperties = json_object_new_object();
            json_object_object_add(poObjCRS, "properties", poObjProperties);

            if (strcmp(pszAuthorityCode, "4326") == 0)
            {
                json_object_object_add(poObjProperties, "name",
                        json_object_new_string("urn:ogc:def:crs:OGC:1.3:CRS84"));
            }
            else
            {
                json_object_object_add(poObjProperties, "name",
                        json_object_new_string(CPLSPrintf("urn:ogc:def:crs:EPSG::%s", pszAuthorityCode)));
            }

            const char* pszCRS = json_object_to_json_string( poObjCRS );
            VSIFPrintfL( fpOut_, "\"crs\": %s,\n", pszCRS );

            json_object_put(poObjCRS);
        }
    }

/* ------------------------------------------------------------------------- */
/*      Serialize Feature                                                    */
/* ------------------------------------------------------------------------- */
    /* the line befor starting write feature data */
    /* control study through geojson outfile */
    VSIFPrintfL( fpOut_, "\"features\": [\n" );

    int firstFlg = 1;
    ///////////////////////////////from OGRGeoJSONWriterLayer::CreateFeature()
    while(true)
    {
        OGRFeature* pSrcFeature = pSrcLayer->GetNextFeature();
        if(!pSrcFeature)
        {
            std::cout<<"OVER"<<std::endl;
            break;
        }
        else
        {
            json_object* poObj = OGRGeoJSONWriteFeature( pSrcFeature, 0, 15);
            if(firstFlg == 1)
            {
                VSIFPrintfL(fpOut_, "\n");
            }
            else
            {
                VSIFPrintfL(fpOut_, ",\n");
            }

            firstFlg = 0;
            VSIFPrintfL( fpOut_, "%s", json_object_to_json_string( poObj ) );
            json_object_put( poObj );
        }
    }

    /////////////////////from OGRGeoJSONWriterLayer::~OGRGeoJSONWriterLayer()
    // tail of GeoJSON file
    VSIFPrintfL( fpOut_, "\n]" );
    VSIFPrintfL( fpOut_, "\n}\n" );


}


/*****************************************************************************/
/*                          SerializeToMemGeoJSON()                         */
/*****************************************************************************/

//BufInfo SerializeToMemGeoJSON(OGRLayer* pSrcLayer)
void DataOperation::SerializeToMemGeoJSON(OGRLayer* pSrcLayer,void* pLocation, int& length)
{
    FILE* fpOut_ = VSIFOpenL( "SerializeToMemGeoJSON.outfile", "w");
    ///////////////////////////////from OGRGeoJSONDataSource::CreateLayer()
    /* begin of geojson file */
    VSIFPrintfL( fpOut_, "{\n\"type\": \"FeatureCollection\",\n" );

/* ------------------------------------------------------------------------- */
/*      Serialize metadata                                                   */
/* ------------------------------------------------------------------------- */
    OGRSpatialReference *poSRS = pSrcLayer->GetSpatialRef();
    if (poSRS)
    {
        const char* pszAuthority = poSRS->GetAuthorityName(NULL);
        const char* pszAuthorityCode = poSRS->GetAuthorityCode(NULL);
        if (pszAuthority != NULL && pszAuthorityCode != NULL &&
                strcmp(pszAuthority, "EPSG") == 0)
        {

            json_object* poObjCRS = json_object_new_object();
            json_object_object_add(poObjCRS, "type",
                    json_object_new_string("name"));

            json_object* poObjProperties = json_object_new_object();
            json_object_object_add(poObjCRS, "properties", poObjProperties);

            if (strcmp(pszAuthorityCode, "4326") == 0)
            {
                json_object_object_add(poObjProperties, "name",
                        json_object_new_string("urn:ogc:def:crs:OGC:1.3:CRS84"));
            }
            else
            {
                json_object_object_add(poObjProperties, "name",
                        json_object_new_string(CPLSPrintf("urn:ogc:def:crs:EPSG::%s", pszAuthorityCode)));
            }

            const char* pszCRS = json_object_to_json_string( poObjCRS );
            VSIFPrintfL( fpOut_, "\"crs\": %s,\n", pszCRS );

            json_object_put(poObjCRS);
        }
    }

/* ------------------------------------------------------------------------- */
/*      Serialize Feature                                                    */
/* ------------------------------------------------------------------------- */
    /* the line befor starting write feature data */
    /* control study through geojson outfile */
    VSIFPrintfL( fpOut_, "\"features\": [\n" );

    int firstFlg = 1;
    ///////////////////////////////from OGRGeoJSONWriterLayer::CreateFeature()
    while(true)
    {
        OGRFeature* pSrcFeature = pSrcLayer->GetNextFeature();
        if(!pSrcFeature)
        {
            std::cout<<"OVER"<<std::endl;
            break;
        }
        else
        {
            json_object* poObj = OGRGeoJSONWriteFeature( pSrcFeature, 0, 15);
            if(firstFlg == 1)
                VSIFPrintfL(fpOut_, ",\n");
            firstFlg = 0;
            VSIFPrintfL( fpOut_, "%s", json_object_to_json_string( poObj ) );
            json_object_put( poObj );
        }
    }

    /////////////////////from OGRGeoJSONWriterLayer::~OGRGeoJSONWriterLayer()
    // tail of GeoJSON file
    VSIFPrintfL( fpOut_, "\n]" );
    VSIFPrintfL( fpOut_, "\n}\n" );

}


/*****************************************************************************/
/*                               GetJsonHeader()                             */
/*****************************************************************************/

//BufInfo SerializeToMemGeoJSON(OGRLayer* pSrcLayer)
void DataOperation::GetJsonHeader(OGRLayer* pSrcLayer,void* pLocation, int& length)
{
    FILE* fpOut_ = VSIFOpenL( "SerializeToMemGeoJSON.outfile", "w");
    ///////////////////////////////from OGRGeoJSONDataSource::CreateLayer()
    /* begin of geojson file */
    VSIFPrintfL( fpOut_, "{\n\"type\": \"FeatureCollection\",\n" );

/* ------------------------------------------------------------------------- */
/*      Serialize metadata                                                   */
/* ------------------------------------------------------------------------- */
    OGRSpatialReference *poSRS = pSrcLayer->GetSpatialRef();
    if (poSRS)
    {
        const char* pszAuthority = poSRS->GetAuthorityName(NULL);
        const char* pszAuthorityCode = poSRS->GetAuthorityCode(NULL);
        if (pszAuthority != NULL && pszAuthorityCode != NULL &&
                strcmp(pszAuthority, "EPSG") == 0)
        {

            json_object* poObjCRS = json_object_new_object();
            json_object_object_add(poObjCRS, "type",
                    json_object_new_string("name"));

            json_object* poObjProperties = json_object_new_object();
            json_object_object_add(poObjCRS, "properties", poObjProperties);

            if (strcmp(pszAuthorityCode, "4326") == 0)
            {
                json_object_object_add(poObjProperties, "name",
                        json_object_new_string("urn:ogc:def:crs:OGC:1.3:CRS84"));
            }
            else
            {
                json_object_object_add(poObjProperties, "name",
                        json_object_new_string(CPLSPrintf("urn:ogc:def:crs:EPSG::%s", pszAuthorityCode)));
            }

            const char* pszCRS = json_object_to_json_string( poObjCRS );
            VSIFPrintfL( fpOut_, "\"crs\": %s,\n", pszCRS );

            json_object_put(poObjCRS);
        }
    }

}

/*****************************************************************************/
/*                          GetJsonFeaturePage()                             */
/*****************************************************************************/

//BufInfo SerializeToMemGeoJSON(OGRLayer* pSrcLayer)
void DataOperation::GetJsonFeaturePage(OGRLayer* pSrcLayer,void* pLocation, int& length)
{
    int firstFlg = 1;
    ///////////////////////////////from OGRGeoJSONWriterLayer::CreateFeature()
    while(true)
    {
        OGRFeature* pSrcFeature = pSrcLayer->GetNextFeature();
        if(!pSrcFeature)
        {
            std::cout<<"OVER"<<std::endl;
            break;
        }
        else
        {
            json_object* poObj = OGRGeoJSONWriteFeature( pSrcFeature, 0, 15);
            if(firstFlg == 1)
                VSIFPrintfL(fpOut_, ",\n");
            firstFlg = 0;
            VSIFPrintfL( fpOut_, "%s", json_object_to_json_string( poObj ) );
            json_object_put( poObj );
        }
    }
}


/*****************************************************************************/
/*                               GetJsonTail()                               */
/*****************************************************************************/

//BufInfo SerializeToMemGeoJSON(OGRLayer* pSrcLayer)
void DataOperation::SerializeToMemGeoJSON(OGRLayer* pSrcLayer,void* pLocation, int& length)
{
    FILE* fpOut_ = VSIFOpenL( "SerializeToMemGeoJSON.outfile", "w");
    ///////////////////////////////from OGRGeoJSONDataSource::CreateLayer()
    /* begin of geojson file */
    VSIFPrintfL( fpOut_, "{\n\"type\": \"FeatureCollection\",\n" );

/* ------------------------------------------------------------------------- */
/*      Serialize metadata                                                   */
/* ------------------------------------------------------------------------- */
    OGRSpatialReference *poSRS = pSrcLayer->GetSpatialRef();
    if (poSRS)
    {
        const char* pszAuthority = poSRS->GetAuthorityName(NULL);
        const char* pszAuthorityCode = poSRS->GetAuthorityCode(NULL);
        if (pszAuthority != NULL && pszAuthorityCode != NULL &&
                strcmp(pszAuthority, "EPSG") == 0)
        {

            json_object* poObjCRS = json_object_new_object();
            json_object_object_add(poObjCRS, "type",
                    json_object_new_string("name"));

            json_object* poObjProperties = json_object_new_object();
            json_object_object_add(poObjCRS, "properties", poObjProperties);

            if (strcmp(pszAuthorityCode, "4326") == 0)
            {
                json_object_object_add(poObjProperties, "name",
                        json_object_new_string("urn:ogc:def:crs:OGC:1.3:CRS84"));
            }
            else
            {
                json_object_object_add(poObjProperties, "name",
                        json_object_new_string(CPLSPrintf("urn:ogc:def:crs:EPSG::%s", pszAuthorityCode)));
            }

            const char* pszCRS = json_object_to_json_string( poObjCRS );
            VSIFPrintfL( fpOut_, "\"crs\": %s,\n", pszCRS );

            json_object_put(poObjCRS);
        }
    }
}
