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
//
DataOperation::DataOperation()
{
}

//
// Destructor
//
DataOperation::~DataOperation()
{
}

/*****************************************************************************/
/*                          MapFieldType()                                   */
/*****************************************************************************/
poenumFieldType MapFieldType(OGRFieldType& pfieldDefn)
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
/*                          MapFieldType()                                   */
/*****************************************************************************/
poenumFieldType MapGeomType(OGRGeomFieldDefn* pgeomDefn)
{
    //XXX:
    return ftGeom;
    //return ftBlob;
}

/*****************************************************************************/
/*                          ConvertLayer()                                   */
/*****************************************************************************/

int ConvertLayer(OGRLayer* pOGRLayer, IPo_FeatureClass* phlsLayer)
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
        IPo_Feature* pDstFeature = phlsLayer->CreateFeature();
        OGRFeature* pSrcFeature = pOGRLayer->GetFeature(fid);

        // Dump fields of a feature.
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
                    value = pSrcFeature->GetFieldAsIntegerList(ifield);
                    break;
                case OFTReal:
                    value = pSrcFeature->GetFieldAsDouble(ifield);
                    break;
                case OFTRealList:
                    value = pSrcFeature->GetFieldAsDoubleList(ifield);
                    break;
                case OFTString:
                    value = pSrcFeature->GetFieldAsString(ifield);
                    break;
                case OFTStringList:
                    value = pSrcFeature->GetFieldAsStringList(ifield);
                    break;
                case OFTWideString:
                    break;
                case OFTWideStringList:
                    break;
                case Binary:
                    value = pSrcFeature->GetFieldAsBinary(ifield);
                    break;
                case Date:
                    value = pSrcFeature->GetFieldAsDate(ifield);
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
            pDstFeature->SetValue(ifield,&value);
        }

        // Dump geometries of a feature.
        for(int igeom=0;igeom<geomCount;++igeom)
        {
            char* geomWkt;
            pSrcFeature->GetGeomFieldRef(igeom)->exportToWkt(&geomWkt);
            pDstFeature->SetValueFromString(igeom+fieldCount,
                                             const_cast<const char*>(geomWkt));
        }
    }
}
