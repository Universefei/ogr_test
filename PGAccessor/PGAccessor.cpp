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

#define PAGESIZE 500

//
//  Constructor
PGAccessor::PGAccessor()
{
    rsltQue_= new BufList();

}

//
// Destructor
PGAccessor::~PGAccessor()
{
    delete rsltQue_;
    OGRSFDriverRegistrar->GetRegistrar()->ReleaseDataSource(pgDS_);


}

/****************************************************************************/
/*                                SetSQL()                                  */
/****************************************************************************/

void PGAccessor::SetSQL(char* sqlstatement)
{
    sqlStatement_ = sqlstatement;
}

/****************************************************************************/
/*                                SetQue()                                  */
/****************************************************************************/

void PGAccessor::SetQue(BufList* buflist)
{
    rsltQue_ = buflist;
}

/****************************************************************************/
/*                                IsQueEmpty()                              */
/****************************************************************************/

bool IsQueEmpty()
{
    return rsltQue_.isEmpty();
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
    FILE*   fpOut_ = VSIFOpenL( pszFilename, "w");

    char*   segBuf = NULL;
    int     segLen = 0;  
    GetJsonHeader(&segBuf,segLen);
    VSIFPrintfL( fpOut_, "%s",segBuf );

    int     endFlg = 0;
    while(!endFlg)
    {
        endFlg = GetJsonFeaturePage(&segBuf,segLen);
        VSIFPrintfL( fpOut_, "%s",segBuf );
    }
}
/*****************************************************************************/
/*                          DumpRsltToJsonOnDisk()                           */
/*****************************************************************************/

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
    
    //
    // Dump Json Header
    //
    char* headerBuf = NULL;
    int bufLen = 0;
    GetJsonHeader(&headerBuf,bufLen);

    //std::pair<char*, int> plistHeaderNode(headerBuf,bufLen);
    std::pair<char*, int> *plistHeaderNode = new pair<char*,int>(headerBuf,bufLen);
    /*
    cout<<"push back header----------------------"<<endl;
    cout<<"header length"<<endl<<plistHeaderNode->second<<endl;
    cout<<"header content"<<endl<<plistHeaderNode->first<<endl;
    */

    rsltQue_->push_back(plistHeaderNode);
    
    //
    // Dump Json Feature Page
    //
    int endFlg = 0;
    int pageCount = 0;
    while(!endFlg)
    {
        pageCount++;
        char* featurePage = NULL;
        int pageLen = 0;
        endFlg = GetJsonFeaturePage(&featurePage, pageLen);
        //std::pair<char*, int> pfeaPage(featurePage,pageLen);
        std::pair<char*, int> *pfeaPage = new pair<char*, int>(featurePage,pageLen);
        /*
        cout<<"prompt from DUmpRsltToJsonOnMemQue ----------------"<<endl;
        cout<<"feature page size: "<<endl<<pfeaPage->second<<endl;
        cout<<"feature page content: "<<endl<<pfeaPage->first<<endl;
        */

        rsltQue_->push_back(pfeaPage);
    }

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

void PGAccessor::GetJsonHeader(char** outFlow, int& retLen)
{
    char pszstackBuf[1024*1024];
    memset(pszstackBuf,0,1024*1024);

    sprintf( pszstackBuf, "{\n\"type\": \"FeatureCollection\",\n" );
    retLen = strlen(pszstackBuf);

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

            sprintf( pszstackBuf+retLen, "\"crs\": %s,\n", pszCRS );
            retLen = strlen(pszstackBuf);

            json_object_put(poObjCRS);
        }
    }

    sprintf( pszstackBuf+retLen, "\n\"features\": [" );
    retLen = strlen(pszstackBuf);

    // fetch first feature
    OGRFeature* pSrcFeature = pRsltLayer_->GetNextFeature();
    json_object* poObj = OGRGeoJSONWriteFeature( pSrcFeature, 0, 15);
    const char* pszfeature = json_object_to_json_string( poObj );
    sprintf( pszstackBuf+retLen, "\n%s", const_cast<char*>(pszfeature) );
    retLen = strlen(pszstackBuf);

    // copy data from stack to heap.
    char* pszheapHead = (char*)malloc(retLen); 
    strcpy(pszheapHead,pszstackBuf);

    *outFlow = pszheapHead;
}

/*****************************************************************************/
/*                          GetJsonFeaturePage()                             */
/*****************************************************************************/

int PGAccessor::GetJsonFeaturePage(char** outFlow, int& retLen)
{
    // allocate stack memory
    char pszstackBuf[1024*1024*8];
    memset(pszstackBuf,0,1024*1024*8);
    char fBuf[1024*100];
    memset(fBuf,0,1024*100);

    retLen = 0;
    int endFlg = 0;
    int firstFlg = 1;
    int featureCount = PAGESIZE;

    // loading feature from OGRLayer to fill one feature page 
    while(featureCount)
    {
        featureCount--;

        OGRFeature* pSrcFeature = pRsltLayer_->GetNextFeature();
        // if no feature left, write json file tail.
        if(!pSrcFeature)
        {
            endFlg = 1;
            sprintf( fBuf, "\n]\n}\n" );
            retLen += strlen(fBuf);
            strcat(pszstackBuf,fBuf);
            memset(fBuf,0,strlen(fBuf));
            break;
        }
        // fetch feature.
        else
        {
            json_object* poObj = OGRGeoJSONWriteFeature( pSrcFeature, 0, 15);
            const char* pszfeature = json_object_to_json_string( poObj );
            sprintf( fBuf, ",\n%s", const_cast<char*>(pszfeature) );
            retLen += strlen( fBuf );
            strcat(pszstackBuf,fBuf);

            json_object_put( poObj );
        }

        memset(fBuf,0,strlen(fBuf));
    }

    // copy data from stack to heap.
    char* pszheapHead = (char*)malloc(retLen);
    strcpy(pszheapHead,pszstackBuf);//XXX:not using memcpy.

    *outFlow = pszheapHead;

    return endFlg;
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

