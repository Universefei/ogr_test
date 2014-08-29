#include <string.h>
#include <sys/stat.h>
#include <iostream>

#include "ogrsf_frmts.h"
#include "cpl_string.h"

#include "ogrgeojsonwriter.h"
#include "ogrgeojsonutils.h"
#include "ogr_spatialref.h"
#include <json.h>

#include "PGAccessor.h"

#define PAGESIZE 1000
#define MEMSIZE  5

//
//  Constructor
PGAccessor::PGAccessor()
{
}

//
// Destructor
PGAccessor::~PGAccessor()
{
}

/****************************************************************************/
/*                                SetSQL()                                  */
/****************************************************************************/

void PGAccessor::SetSQL(char* sqlstatement)
{
    sqlStatement_ = sqlstatement;
}

/****************************************************************************/
/*                                SetSQL()                                  */
/****************************************************************************/

void PGAccessor::SetQue(BufList* buflist)
{
    rsltQue_ = buflist;
}

/****************************************************************************/
/*                        genFeatureFile()                                  */
/****************************************************************************/

FileInfo PGAccessor::genFeatureFile(const std::string& layerName, 
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
                         // boundingBox.xmin, boundingBox.ymin, 
                         // boundingBox.xmax, boundingBox.ymax
                         );
    std::cout<<queryClause->c_str()<<std::endl;

    // Get a OGRPGResultLayer via executing SQL statement
    OGRLayer  *poPgLayer = poPgDS->ExecuteSQL(queryClause->c_str(), NULL, NULL);
    if(NULL == poPgLayer)
    {
        std::cout<<"executeSQL error"<<std::endl;
    }
    delete(queryClause);


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
/*                               ExecuteSQL()                               */
/****************************************************************************/

OGRLayer* PGAccessor::ExecuteSQL()
{

    OGRLayer* pRetLayer = NULL;
    if(!sqlStatement_)
    {
        std::cout<<"no sql statement to query"<<std::endl;
    }
    else
    {
        pRetLayer = pgDS_->ExecuteSQL(sqlStatement_, NULL, NULL);
    }
    return pRetLayer;
}

/****************************************************************************/
/*                               ExecuteSQL()                               */
/****************************************************************************/
OGRLayer* PGAccessor::ExecuteSQL(const char* sqlStatement)
{
    OGRLayer* pRetLayer = NULL;
    if(!sqlStatement)
    {
        std::cout<<"no sql statement to query"<<std::endl;
    }
    else
    {
        sqlStatement_ = const_cast<char*>(sqlStatement);
        pRetLayer = pgDS_->ExecuteSQL(sqlStatement_, NULL, NULL);

    }
    pRsltLayer_ = pRetLayer;
    return pRetLayer;
}

/*****************************************************************************/
/*                                      ConnDB()                             */
/*****************************************************************************/

OGRDataSource* PGAccessor::ConnDB( const char* host,
                                const char* port,
                                const char* user,
                                const char* password,
                                const char* dbname )
{
    char* pszConnInfo = (char*)malloc(sizeof(char)*200);
    sprintf(pszConnInfo,"PG: host='%s' port='%s' user='%s' password='%s' dbname='%s'",
                            host,port,user,password,dbname);

    OGRRegisterAll();
    const char *pszPGDriverName = "PostgreSQL";
    OGRSFDriver* poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszPGDriverName);

    OGRDataSource *poPgDS = poDriver->Open(pszConnInfo);

    pgDS_ = poPgDS;
    return poPgDS;
}

/*****************************************************************************/
/*                                ShowRsltOnTerm()                           */
/*****************************************************************************/
void PGAccessor::ShowRsltOnTerm()
{
    printf( "{\n\"type\": \"FeatureCollection\",\n" );
/* ------------------------------------------------------------------------- */
/*      Serialize metadata                                                   */
/* ------------------------------------------------------------------------- */
    OGRSpatialReference *poSRS = pRsltLayer_->GetSpatialRef();
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
            printf( "\"crs\": %s,\n", pszCRS );

            json_object_put(poObjCRS);
        }
    }
/* ------------------------------------------------------------------------- */
/*      Serialize Feature                                                    */
/* ------------------------------------------------------------------------- */
    printf( "\"features\": [\n" );

    int firstFlg = 1;
    while(true)
    {
        OGRFeature* pSrcFeature = pRsltLayer_->GetNextFeature();
        if(!pSrcFeature)
        {
            break;
        }
        else
        {
            json_object* poObj = OGRGeoJSONWriteFeature( pSrcFeature, 0, 15);
            if(firstFlg == 1)
            {
                printf( "\n");
            }
            else
            {
                printf( ",\n");
            }

            firstFlg = 0;
            printf( "%s", json_object_to_json_string( poObj ) );
            json_object_put( poObj );
        }
    }
    // tail of GeoJSON file
    printf( "\n]" );
    printf( "\n}\n" );
}

/*****************************************************************************/
/*                          DumpRsltToJsonOnDisk()                           */
/*****************************************************************************/

const char* PGAccessor::DumpRsltToJsonOnDisk(const char* pszFilename)
{
    cout <<"start---------------------------------"<<endl;
    FILE* fpOut_ = VSIFOpenL( pszFilename, "w");
    
    cout <<"Header---------------------------------"<<endl;
    VSIFPrintfL( fpOut_, "{\n\"type\": \"FeatureCollection\",\n" );
/* ------------------------------------------------------------------------- */
/*      Serialize metadata                                                   */
/* ------------------------------------------------------------------------- */
    cout <<"SRS---------------------------------"<<endl;
    OGRSpatialReference *poSRS = pRsltLayer_->GetSpatialRef();
    cout <<"GetSpatialRef---------------------------------"<<endl;
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
    cout <<"start features---------------------------------"<<endl;
    VSIFPrintfL( fpOut_, "\"features\": [\n" );

    int firstFlg = 1;
    while(true)
    {
        OGRFeature* pSrcFeature = pRsltLayer_->GetNextFeature();
        if(!pSrcFeature)
        {
            std::cout<<"OVER"<<std::endl;
            break;
        }
        else
        {
            //cout <<"start features---------------------------------"<<pSrcFeature->GetFID()<<endl;

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
    VSIFPrintfL( fpOut_, "\n]" );
    VSIFPrintfL( fpOut_, "\n}\n" );
    cout <<"start features--------------------------over   "<<endl;

    return pszFilename;
}

/*****************************************************************************/
/*                          DumpRsltToJsonOnMemQue()                         */
/*****************************************************************************/

BufList* PGAccessor::DumpRsltToJsonOnMemQue()
{
    if(!rsltQue_)
    {
        std::cout<<"BufList is Null"<<std::endl;
        return NULL;
    }
    
    char* headerBuf = NULL;
    int bufLen = 0;
    GetJsonHeader(&headerBuf,&bufLen);

    std::pair<const char*, int> plistHeaderNode(headerBuf,bufLen);
    cout<<"push back header----------------------"<<endl;
    rsltQue_->push_back(&plistHeaderNode);
    
    int endFlg = 0;
    int pageCount = 0;
    while(!endFlg)
    {
        pageCount++;
        char* featurePage = NULL;
        int pageLen = 0;
        endFlg = GetJsonFeaturePage(&featurePage, &pageLen);
        std::pair<const char*, int> pfeaPage(featurePage,pageLen);
        //cout<<"push back Page----------------------"<<pageCount<<endl;
        rsltQue_->push_back(&pfeaPage);
    }

    cout<<"push back over----------------------"<<endl;

    return rsltQue_;

}

/*****************************************************************************/
/*                               ConvertRsltToObj()                          */
/*****************************************************************************/
void* PGAccessor::ConvertRsltToObj()
{

}

/*****************************************************************************/
/*                               GetJsonHeader()                             */
/*****************************************************************************/

void PGAccessor::GetJsonHeader(char** outFlow, int* retLen)
{
    *outFlow = (char*)malloc(sizeof(char)*1024*1024*MEMSIZE);

    sprintf( *outFlow, "{\n\"type\": \"FeatureCollection\",\n" );

/* ------------------------------------------------------------------------- */
/*      Serialize metadata                                                   */
/* ------------------------------------------------------------------------- */
    OGRSpatialReference *poSRS = pRsltLayer_->GetSpatialRef();
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
            sprintf( *outFlow, "\"crs\": %s,\n", pszCRS );

            json_object_put(poObjCRS);
        }
    }

    *retLen = strlen(*outFlow);
}

/*****************************************************************************/
/*                          GetJsonFeaturePage()                             */
/*****************************************************************************/

int PGAccessor::GetJsonFeaturePage(char** outFlow, int* retLen)
{
    *outFlow = (char*)malloc(sizeof(char)*1024*1024*MEMSIZE);

    int endFlg = 0;
    int firstFlg = 1;
    int featureCount = PAGESIZE;
    *retLen = 0;

    while(featureCount)
    {
        featureCount--;

        OGRFeature* pSrcFeature = pRsltLayer_->GetNextFeature();
        // if no feature left
        if(!pSrcFeature)
        {
            endFlg = 1;
            sprintf( *outFlow, "\n]" );
            sprintf( *outFlow, "\n}\n" );
            break;
        }
        else
        {
            json_object* poObj = OGRGeoJSONWriteFeature( pSrcFeature, 0, 15);
            if(firstFlg == 1)
            {
                sprintf(*outFlow, "\n");
                *retLen++;
                firstFlg = 0;
            }
            else
            {
                sprintf(*outFlow, ",\n");
                *retLen += 2;
            }
            const char* pszfStr = json_object_to_json_string( poObj );

            *retLen += strlen(pszfStr);
            sprintf( *outFlow, "%s", pszfStr );
            json_object_put( poObj );
        }
    }
    return endFlg;
}

