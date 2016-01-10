#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"
#include "metadata.h"
#include "ogrvtlayer.h"
#include "ogrsf_frmts.h"
#include <cstring>
#include <string>
#include <vector>


using namespace std;
using namespace rapidjson;


OGRwkbGeometryType getGeometryTypeFromStr(const char* g_type) {
    if(!strcmp(g_type, "wkbPoint")) return wkbPoint;
    else if(!strcmp(g_type, "wkbPolygon")) return wkbPolygon;
    else if(!strcmp(g_type, "wkbLineString")) return wkbLineString;

    return wkbUnknown;
}


OGRFieldType getFieldTypeFromStr(const char* g_type) {
    if(!strcmp(g_type, "int")) return OFTInteger;
    else if(!strcmp(g_type, "string")) return OFTString;
    else if(!strcmp(g_type, "real")) return OFTReal;

    return OFTString;
}


typedef struct {
    const char* pszName;
    const char* pszType;
} FieldInfo;


OGRFieldDefn* generateFieldDefn(FieldInfo* poFieldInfo) 
{
    return new OGRFieldDefn(poFieldInfo->pszName, 
            getFieldTypeFromStr( poFieldInfo->pszType));
}


OGRVTLayer* generateLayer(const char* layer_meta_path)
{
    FILE* fp = fopen(layer_meta_path, "r");
    char readBuffer[65536];
    FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    Document d;
    d.ParseStream(is);

    const char* pszLayerName = d["name"].GetString();
    int srid = d["srid"].GetInt();
    const char* geo_type = d["geometry_type"].GetString();
    /* char* geo_column = "coordinate"; */
    vector<FieldInfo> fields;
    const Value& flds = d["fields"];
    for(SizeType i = 0; i < flds.Size(); ++i)
    {
        FieldInfo finfo;
        finfo.pszName = flds[i]["name"].GetString();
        finfo.pszType = flds[i]["type"].GetString();
        fields.push_back(finfo);
    }

    OGRSpatialReference *poSRS = new OGRSpatialReference();
    poSRS->importFromEPSG(srid);
    OGRwkbGeometryType eType = getGeometryTypeFromStr(geo_type);

    OGRVTLayer *poLayer = new OGRVTLayer( pszLayerName, poSRS, eType );
    OGRFeatureDefn *poFeatureDefn = poLayer->GetLayerDefn();

    vector<FieldInfo>::iterator itr;
    for(itr = fields.begin(); itr != fields.end(); itr++) 
    {
        poFeatureDefn->AddFieldDefn(generateFieldDefn( &(*itr) ));
    }

    fclose(fp);
    return poLayer;
}


vector<string> *getLayerList(const char* path)
{
    FILE* fp = fopen(path, "r");
    char readBuffer[65536];
    FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    Document d;
    d.ParseStream(is);

    const Value& list = d["layers"];
    vector<string> * ret = new vector<string>;
    for(SizeType i = 0; i < list.Size(); ++i)
    {
        ret->push_back(string(list[i].GetString()));
    }

    fclose(fp);
    return ret;
}
