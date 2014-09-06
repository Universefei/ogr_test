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

/****************************************************************************/
/*                              PGAccessor()                                */
/****************************************************************************/
//  Constructor
PGAccessor::PGAccessor()
{
    rsltQue_= new BufList();
    pRsltLayer_ = NULL;
}

/****************************************************************************/
/*                             ~PGAccessor()                                */
/****************************************************************************/
// Destructor
PGAccessor::~PGAccessor()
{
    delete rsltQue_;
    OGRSFDriverRegistrar::GetRegistrar()->ReleaseDataSource(pgDS_);
}

/****************************************************************************/
/*                                SetSQL()                                  */
/****************************************************************************/

void PGAccessor::SetSQL(char* sqlstatement)
{
    sqlStatement_ = sqlstatement;
    ExecuteSQL();
}

/****************************************************************************/
/*                                SetSQL()                                  */
/****************************************************************************/

void PGAccessor::SetSQL(const string& layerName, 
        const BBOX& boundingBox, const fieldVec& attrColumn)
{
    
    // make all column name in one string
    string columnStr = "";
    int i=0;
    while (i<attrColumn.size()-1)
    {
        columnStr+=attrColumn[i];
        columnStr+=",";
        i++;
    }
    columnStr+=attrColumn[i];

    //cout<<__LINE__<<endl;
    char* pszsqlstat=(char*)malloc(1024*10);
    sprintf(pszsqlstat,"SELECT %s FROM %s \
                                  WHERE ST_Contains( ST_MakeEnvelope(%f,%f,%f,%f)::geography::geometry,way)",
                         columnStr.c_str(), 
                         layerName.c_str() ,
                         boundingBox.xmin, boundingBox.ymin, 
                         boundingBox.xmax, boundingBox.ymax
                         );

    sqlStatement_ = pszsqlstat;
    ExecuteSQL();
}

/****************************************************************************/
/*                              GetRsltList()                               */
/****************************************************************************/

BufList* PGAccessor::GetRsltList()
{
    return rsltQue_;
}

/****************************************************************************/
/*                                IsQueEmpty()                              */
/****************************************************************************/

bool PGAccessor::IsQueEmpty()
{
    pthread_mutex_lock(&mutex_);
    bool isEmpty = rsltQue_->empty();
    pthread_mutex_unlock(&mutex_);
    return isEmpty;
}

/****************************************************************************/
/*                               GetNextJsonSeg()                           */
/****************************************************************************/

/**
 * @brief:  pop a node from result list.
 *
 * @return: 1 if reach the end and returned pair is unuseable, else return 0.
 * @ppair:  a BufList node get from the front.
 */
int PGAccessor::GetNextJsonSeg(pair<char*,int>** ppair)
{
    pthread_mutex_lock(&mutex_);
    *ppair = rsltQue_->front();
    rsltQue_->pop_front();
    pthread_mutex_unlock(&mutex_);

    // check if reach the end.
    if( NULL==(*ppair)->first && (*ppair)->second==0 )
    {
        return 1;
    }

    return 0;
}

/****************************************************************************/
/*                               ExecuteSQL()                               */
/****************************************************************************/

OGRLayer* PGAccessor::ExecuteSQL()
{

    OGRLayer* pRetLayer = NULL;
    if(!sqlStatement_)
    {
        cout<<"no sql statement to query"<<endl;
    }
    else
    {
        //cout<<"before executing:"<<endl<<sqlStatement_<<endl;
        pRetLayer = pgDS_->ExecuteSQL(sqlStatement_, NULL, NULL);
        //cout<<"executing:"<<endl<<sqlStatement_<<endl;
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
    char* pszConnInfo = (char*)malloc(sizeof(char)*1000);
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
    int     pageCount = 0;
    while(!endFlg)
    {
        endFlg = GetJsonFeaturePage(&segBuf,segLen);
        VSIFPrintfL( fpOut_, "%s",segBuf );
    }

    //fclose(fpOut_);
}
/*****************************************************************************/
/*                          DumpRsltToJsonOnMemQue()                         */
/*****************************************************************************/

BufList* PGAccessor::DumpRsltToJsonOnMemQue()
{
    if(!rsltQue_)
    {
        cout<<"BufList is Null"<<std::endl;
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
    pthread_mutex_lock(&mutex_);
    rsltQue_->push_back(plistHeaderNode);
    pthread_mutex_unlock(&mutex_);
    
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
        pthread_mutex_lock(&mutex_);
        rsltQue_->push_back(pfeaPage);
        pthread_mutex_unlock(&mutex_);
    }

    //
    // push back Json end segement flag
    //
    pair<char*, int> *pendSegFlag = new pair<char*, int>(NULL,0);
    pthread_mutex_lock(&mutex_);
    rsltQue_->push_back(pendSegFlag);
    pthread_mutex_unlock(&mutex_);

    return rsltQue_;

}

/*****************************************************************************/
/*                               DumpToHlsLayer()                            */
/*****************************************************************************/
int PGAccessor::DumpToHlsLayer(IPo_FeatureClass* phlsLayer)
{
    if(NULL==phlsLayer)
    {
        cout<<"phlsLayer is Null"<<std::endl;
        return 0;
    }

    return ConvertLayer(prsltLayer_,phlsLayer);
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
/*                               GetJsonHeader()                             */
/*****************************************************************************/

void PGAccessor::GetJsonHeader(char** outFlow, int& retLen)
{
    char pszstackBuf[1024*1024*5];
    memset(pszstackBuf,0,1024*1024*5);

    sprintf( pszstackBuf, "{\n\"type\": \"FeatureCollection\",\n" );
    retLen = strlen(pszstackBuf);

    //
    // Serialize metadata                                       
    //
    if(NULL==pRsltLayer_)
        return;
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

    //
    // fetch first feature
    //
    OGRFeature* pSrcFeature = pRsltLayer_->GetNextFeature();
    if(!pSrcFeature)
    {
        json_object* poObj = OGRGeoJSONWriteFeature( pSrcFeature, 0, 15);
        const char* pszfeature = json_object_to_json_string( poObj );
        sprintf( pszstackBuf+retLen, "\n%s", const_cast<char*>(pszfeature) );
        retLen = strlen(pszstackBuf);
    }

    // copy data from stack to heap.
    char* pszheapHead = (char*)malloc(retLen); 
    memset(pszheapHead,0,retLen);
    strcpy(pszheapHead,pszstackBuf);

    *outFlow = pszheapHead;
}

/*****************************************************************************/
/*                          GetJsonFeaturePage()                             */
/*****************************************************************************/

int PGAccessor::GetJsonFeaturePage(char** outFlow, int& retLen)
{
    if(NULL==pRsltLayer_)
        return -1;
    // allocate temp memory
    char* pszpageBuf = (char*)malloc(1024*1024*12);
    memset(pszpageBuf,0,1024*1024*12);
    char fBuf[1024*1024*3];
    memset(fBuf,0,1024*1024*3);

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
            strcat(pszpageBuf,fBuf);
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
            strcat(pszpageBuf,fBuf);

            json_object_put( poObj );
        }

        memset(fBuf,0,strlen(fBuf));
    }

    char* pszheapHead = (char*)malloc(retLen);
    strcpy(pszheapHead,pszpageBuf);
    *outFlow = pszheapHead;
    free(pszpageBuf);
    return endFlg;
}

/*****************************************************************************/
/*                          MapFieldType()                                   */
/*****************************************************************************/
poenumFieldType PGAccessor::MapFieldType(OGRFieldType& pfieldDefn)
{
    poenumFieldType retFieldType;
    switch(pfieldDefn)
    {
        case OFTInteger:
            retFieldType = ftInteger;
            break;
        case OFTIntegerList:
            //retFieldType = ftInteger;
            break;
        case OFTReal:
            retFieldType = ftSingle;
            break;
        case OFTRealList:
            //retFieldType = ftInteger;
            break;
        case OFTString:
            retFieldType = ftString;
            break;
        case OFTStringList:
            //retFieldType = ftInteger;
            break;
        case OFTWideString:
            retFieldType = ftLongString;
            break;
        case OFTWideStringList:
            //retFieldType = ftInteger;
            break;
        case Binary:
            retFieldType = ftBinary;
            break;
        case Date:
            retFieldType = ftDate;
            break;
        case Time:
            //retFieldType = ftInteger;
            break;
        case DateTime:
            //retFieldType = ftInteger;
            break;
        case MaxType:
            //retFieldType = ftInteger;
            break;
        default:

    }
    return retFieldType;
}

/*****************************************************************************/
/*                          MapGeomType()                                   */
/*****************************************************************************/
poenumFieldType PGAccessor::MapGeomType(OGRGeomFieldDefn* pgeomDefn)
{
    //TODO:
    return ftGeom;
    //return ftBlob;
}

/*****************************************************************************/
/*                          ConvertLayer()                                   */
/*****************************************************************************/

int PGAccessor::ConvertLayer(OGRLayer* pOGRLayer, IPo_FeatureClass* phlsLayer)
{
    if(NULL==pOGRLayer || NULL==phlsLayer)
    {
        std::cout<<"not specify input or output Layer"<<std::endl;
    }

    OGRFeatureDefn* pOGRFeatureDefn = pOGRLayer->GetLayerDefn();
    IPo_TableFieldSet* phlsFSet = phlsLayer->Fields();

    phlsLayer->DeleteFeaturesAll();
    phlsLayer->DeleteFieldsAll();

    //
    // Dump Attributes fields definitions 
    //
    int fieldCount = pOGRFeatureDefn->GetFieldCount();
    OGRFieldDefn* pFieldDefn = NULL;
    for(int idx=0; idx<fieldCount; ++idx)
    {
        pFieldDefn = pOGRFeatureDefn->GetFieldDefn(idx);

        char* szname = const_cast<char*>( pFieldDefn->GetNameRef() );
        poenumFieldType efieldType = MapFieldType(pFieldDefn->GetType());
        unsigned int uiLength = pFieldDefn->GetWidth();
        char* szAliasName = NULL;
        long lPrecision = pFieldDefn->GetPrecision();
        bool bNullable = true;
        char* pzDesc = NULL;

        phlsLayer->AddField(szname,efieldType, uiLength, 
                szAliasName,lPrecision,bNullable, pzDesc);
    }

    //
    // XXX:Dump Geometry fields definitions
    //
    int geomCount = pOGRFeatureDefn->GetGeomFieldCount();
    OGRGeomFieldDefn* pGeomDefn = NULL;
    for(int idx=0; idx<geomCount; ++idx)
    {
        pGeomDefn = pOGRFeatureDefn->GetGeomFieldDefn(idx);

        char* szname = const_cast<char*>( pGeomDefn->GetNameRef() );
        poenumFieldType efieldType = MapGeomType(pGeomDefn->GetType());
        unsigned int uiLength = pGeomDefn->GetWidth();
        char* szAliasName = NULL;
        long lPrecision = pGeomDefn->GetPrecision();
        bool bNullable = true;
        char* pzDesc = NULL;

        hlsLayer->AddField(szname,efieldType, uiLength, 
                szAliasName,lPrecision,bNullable, pzDesc);
    }

    //
    // Dump features.
    //
    int fCount = pOGRLayer->GetFeatureCount(); 
    OGRFieldDefn* pfieldDefn = NULL;
    for(int fid=0;fid<fCount;++fid)
    {
        IPo_Feature* pDstFeature = phlsLayer->CreateFeature();
        OGRFeature* pSrcFeature = pOGRLayer->GetFeature(fid);

        //
        // Dump fields of a feature.
        //
        for(int ifield=0;ifield<fieldCount;++ifield)
        {
            //pfieldDefn = pOGRFeatureDefn->GetFieldDefn(ifield);
            char* pszvalue = pSrcFeature->GetFieldAsString(ifield);
            pDstFeature->SetValueFromString(ifield,pszvalue);
        }

        //
        // Dump geometries of a feature.
        //
        OGRGeomFieldDefn* pGeomDf = NULL;
        IGeo_Geometry * pgeom;
        for(int igeom=0;igeom<geomCount;++igeom)
        {
            pGeomDf = pSrcFeature->GetGeomFieldRef(igeom);
            switch(pGeomDf->GetType())
            {
                case wkbUnknown:
                    break;
                case wkbPoint:
                    {
                        IGeo_Point* pDstGeom = NULL;
                        CoCreateGeometryObject(GEO_TYPE_GEOPOINT,(void**)&pDstGeom);

                        OGRPoint* pSrcGeom = dynamic_cast<OGRPoint*>(
                                pSrcFeature->GetGeomFieldRef(igeom) ); 
                        if(pSrcGeom)
                        {
                            pDstGeom->SetPoint( SPo_DPoint(pSrcGeom->getX(),
                                        pSrcGeom->getY(),pSrcGeom->getZ()));
                        }
                        pDstFeature->SetGeoPtr(pDstGeom);
                        break;
                    }
                case wkbLineString:
                    {
                        IGeo_LineString* pDstGeom = NULL;
                        CoCreateGeometryObject(GEO_TYPE_GEOLINESTRING,(void**)&pDstGeom);

                        OGRLineString* pSrcGeom = dynamic_cast<OGRLineString*>(
                                pSrcFeature->GetGeomFieldRef(igeom) ); 
                        if(pSrcGeom)
                        {
                            // Dump every point here
                            int pointCount = pSrcGeom->getNumPoints();
                            for(int i=0;i<pointCount;++i)
                            {
                                IGeo_Point* pPoint = NULL;
                                CoCreateGeometryObject(GEO_TYPE_GEOPOINT,(void**)&pPoint);
                                pPoint->SetPoint( SPo_DPoint(pSrcGeom->getX(i),
                                            pSrcGeom->getY(i),pSrcGeom->getZ(i)));
                                pDstGeom->AddElement(pPoint);
                            }
                        }
                        pDstFeature->SetGeoPtr(pDstGeom);
                        break;
                    }
                case wkbPolygon:
                    {
                        IGeo_Polygon* pDstGeom = NULL;
                        CoCreateGeometryObject(GEO_TYPE_GEOPOLYGON,(void**)&pDstGeom);

                        OGRPolygon* pSrcGeom = dynamic_cast<OGRPolygon*>(
                                pSrcFeature->GetGeomFieldRef(igeom) ); 
                        if(pSrcGeom)
                        {
                            // XXX:Dump OGR polygon which has only one exterior ring.
                            // Get External LineRing.
                            OGRLinearRing* ptmpLRing = NULL;
                            ptmpLRing = pSrcGeom->GetExteriorRing();
                            int pointCnt = ptmpLRing->GetNumPoints();
                            for(int i=0;i<pointCnt;++i)
                            {
                                pDstGeom->AddPoint(SPo_DPoint(tmpLRing->getX(i),
                                            tmpLRing->getY(i)),tmpLRing->getX(i));
                            }
                            // Get Internal LineRings.
                            ptmpLRing = NULL;
                            pointCnt = 0;
                            int inLRCnt = pSrcGeom->getNumInteriorRings();
                            for ( int j = 0 ; j < inLRCnt; ++j)
                            {
                                ptmpLRing = pSrcGeom->getInteriorRing(j);
                                pointCnt = ptmpLRing->GetNumPoints();
                                for ( int k = 0 ; k < pointCnt; k++ )
                                {
                                    pDstGeom->AddPoint(SPo_DPoint(tmpLRing->getX(i),
                                                tmpLRing->getY(i)),tmpLRing->getX(i));
                                }
                            } 
                            // Set feature geometry data.
                            pDstFeature->SetGeoPtr(pDstGeom);
                        }
                        break;
                    }
                case wkbMultiPoint:
                    {
                        IGeo_MultiPoint* pDstGeom = NULL;
                        CoCreateGeometryObject(GEO_TYPE_GEOMULTIPOINT,(void**)&pDstGeom);

                        OGRMultiPoint* pSrcGeom = dynamic_cast<OGRMultiPoint*>(
                                pSrcFeature->GetGeomFieldRef(igeom) ); 
                        if(pSrcGeom)
                        {
                            // Dump all geometry data here.
                            int geomCount = pSrcGeom->getNumGeometries(); 
                            for(int i=0;i<geomCount;++i)
                            {
                                OGRPoint* pSrcPoint = dynamic_cast<OGRPoint*>( 
                                        pSrcGeom->getGeometryRef(i) );
                                if(pSrcPoint)
                                {
                                    pDstGeom->AddPoint(i,SPo_DPoint(pSrcPoint->getX(),
                                                pSrcPoint->getY(),pSrcPoint->getz()));
                                }
                            }
                        }
                        pDstFeature->SetGeoPtr(pDstGeom);
                        break;
                    }
                case wkbMultiLineString:
                    {
                        IGeo_MultiLineString* pDstGeom = NULL;
                        CoCreateGeometryObject(GEO_TYPE_GEOMULTILINESTRING,(void**)&pDstGeom);

                        OGRMultiLineString* pSrcGeom = dynamic_cast<OGRMultiLineString*>(
                                pSrcFeature->GetGeomFieldRef(igeom) ); 
                        if(pSrcGeom)
                        {
                            int geomCount = pSrcGeom->getNumGeometries(); 
                            for(int i=0;i<geomCount;++i)
                            {
                                // Dump one line string.
                                IGeo_LineString* pDstLString = NULL;
                                CoCreateGeometryObject(GEO_TYPE_GEOLINESTRING,(void**)&pDstLString);
                                OGRLineString* pSrcLString = dynamic_cast<OGRLineString*>( 
                                        pSrcGeom->getGeometryRef(i) );
                                if(pSrcLString)
                                {
                                    int pointCount = pSrcLString->getNumPoints();
                                    for(int j=0;i<pointCount;++j)
                                    {
                                        IGeo_Point* pPoint = NULL;
                                        CoCreateGeometryObject(GEO_TYPE_GEOPOINT,(void**)&pPoint);
                                        pPoint->SetPoint( SPo_DPoint(pSrcLString->getX(j),
                                                    pSrcLString->getY(j),pSrcLString->getZ(j)));
                                        pDstLString->AddElement(pPoint);
                                    }

                                    pDstGeom->AddLineString(pSrcLString);
                                }
                            }
                            // Set Geometry of feature.
                            pDstFeature->SetGeoPtr(pDstGeom);
                        }
                        break;
                    }
                case wkbMultiPolygon:
                    {
                        IGeo_MultiPolygon* pDstGeom = NULL;
                        CoCreateGeometryObject(GEO_TYPE_GEOPOLYGON,(void**)&pDstGeom);

                        OGRMultiPolygon* pSrcGeom = dynamic_cast<OGRMultiPolygon*>(
                                pSrcFeature->GetGeomFieldRef(igeom) ); 
                        if(pSrcGeom)
                        {
                            // Traverse every polygon in OGR multipolygon.
                            int geomCount = pSrcGeom->getNumGeometries(); 
                            for(int i=0;i<geomCount;++i)
                            {
                                // Get a polygon in Multipolygon.
                                OGRPolygon* pChildPolygon = dynamic_cast<OGRPolygon*>(
                                        pSrcGeom->getGeometryRef(i) );
                                if(!pChildPolygon) { break; }

                                OGRLinearRing* ptmpLRing = NULL;
                                int pointCnt = 0;

                                // Get Exterior LineRing of fetched polygon.
                                ptmpLRing = pChildPolygon->GetExteriorRing();
                                pointCnt = ptmpLRing->GetNumPoints();
                                for(int i=0;i<pointCnt;++i)
                                {
                                    pDstGeom->AddPoint(SPo_DPoint(tmpLRing->getX(i),
                                                tmpLRing->getY(i)),tmpLRing->getX(i));
                                }

                                // Get Interior LineRings of fetched polygon.
                                ptmpLRing = NULL;
                                pointCnt = 0;
                                int inLRCnt = pSrcGeom->getNumInteriorRings();
                                for ( int j = 0 ; j < inLRCnt; ++j)
                                {
                                    ptmpLRing = pChildPolygon->getInteriorRing(j);
                                    pointCnt = ptmpLRing->GetNumPoints();
                                    for ( int k = 0 ; k < pointCnt; k++ )
                                    {
                                        pDstGeom->AddPoint(SPo_DPoint(tmpLRing->getX(i),
                                                 tmpLRing->getY(i)),tmpLRing->getX(i));
                                    }
                                } 
                            }
                            // Set geometry field of destinate feature.
                            pDstFeature->SetGeoPtr(pDstGeom);
                        }
                        break;
                    }
                case wkbGeometryCollection:
                    {
                        // TODO:Dump geometry collection here.
                        break;
                    }
                default:
                    break;
            } 

        }// end of converting geometry.
    } // end of converting feature.
    return 1;
}
