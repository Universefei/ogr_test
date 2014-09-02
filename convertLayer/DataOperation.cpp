#include "ogrsf_frmts.h"
#include "cpl_string.h"
#include "DataOperation.h"
#include <iostream>
#include <string.h>
#include <sys/stat.h>
#include <ogr_core.h>
#include "VectorEngine/IPo_FeatureClass.h"
#include "VectorEngine/IPo_TableFieldSet.h"

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

poenumFieldType MapFieldType(OGRFieldType& pfieldDefn)
{
    poenumFieldType retFieldType;

    switch(pfieldDefn)
    {
        case OFTInteger:
            retFieldType = ftInteger;
            break;
        case OFTIntegerList:
            retFieldType = ftInteger;
            break;
        case OFTReal:
            retFieldType = ftInteger;
            break;
        case OFTRealList:
            retFieldType = ftInteger;
            break;
        case OFTString:
            retFieldType = ftInteger;
            break;
        case OFTStringList:
            efieldType = ftInteger;
            break;
        case OFTWideString:
            efieldType = ftInteger;
            break;
        case OFTWideStringList:
            efieldType = ftInteger;
            break;
        case Binary:
            efieldType = ftInteger;
            break;
        case Date:
            break;
        case Time:
            break;
        case DateTime:
            break;
        case MaxType:
            break;
        default:

    }
}

poenumFieldType MapGeomType(OGRGeomFieldDefn* pgeomDefn)
{
    //FIXME:
    //XXX:
}

/*****************************************************************************/
/*                          ConvertLayer()                                   */
/*****************************************************************************/

int ConvertLayer(OGRLayer* pOGRLayer, IPo_FeatureClass* phlsLayer)
{
    // parameters check
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
        // TODO: add fields definition.
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
    // Dump Geometry fields definitions
    //
    int geomCount = pOGRFeatureDefn->GetGeomFieldCount();
    for(int idx=0; idx<geomCount; ++idx)
    {
        // TODO: add geometry definition.
        pFieldDefn = pOGRFeatureDefn->GetGeomFieldDefn(idx);

        char* szname = const_cast<char*>( pFieldDefn->GetNameRef() );
        poenumFieldType efieldType = MapGeomType(pFieldDefn->GetType());
        unsigned int uiLength = pFieldDefn->GetWidth();
        char* szAliasName = NULL;
        long lPrecision = pFieldDefn->GetPrecision();
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
        IPo_Feature* pDistFeature = phlsLayer->CreateFeature();
        OGRFeature* pSrcFeature = pOGRLayer->GetFeature(fid);

        // Dump OGR fields of a feature.
        for(int ifield=0;ifield<fieldCount;++ifield)
        {
            pfieldDefn = pOGRFeatureDefn->GetFieldDefn(ifield);

            void* value;
            switch( pfieldDefn->GetType() )
            {
                case OFTInteger:
                    value = pSrcFeature->GetFieldAsInteger(ifield);
                    break;
                case OFTIntegerList:
                    break;
                case OFTReal:
                    break;
                case OFTRealList:
                    break;
                case OFTString:
                    break;
                case OFTStringList:
                    break;
                case OFTWideString:
                    break;
                case OFTWideStringList:
                    break;
                case Binary:
                    break;
                case Date:
                    break;
                case Time:
                    break;
                case DateTime:
                    break;
                case MaxType:
                    break;
                default:


            }
            // TODO:fill in one field value.
            SetValue(ifield,&value)
        }

        // Dump geometries of a feature.
        for(int igeom=0;igeom<geomCount;++igeom)
        {
            pfieldDefn = pOGRFeatureDefn->GetFieldDefn(ifield);

        }
    }
}



