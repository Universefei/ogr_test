#include "ogr_vt.h"
#include "ogr_feature.h"
#include "vectortile.h"
#include "ogrvtlayer.h"

#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"

#include <iostream> // for debug
#include <cstring>
using namespace rapidjson;

/* --------------------------------------------------------------------- */
/*                          VectorTile()                                 */
/* --------------------------------------------------------------------- */

VectorTile::VectorTile(OGRVTLayer* poLayer, TileID* poTileID):
    poTileID_( poTileID ? ( new TileID(*poTileID) ) : NULL ),
    poLayer_(poLayer)
{
}

VectorTile::VectorTile(OGRVTLayer* poLayer, int z, int x, int y)
{
    poTileID_ = new TileID(poLayer->GetName(), z, x, y);
    poLayer_ = poLayer;
}

/* --------------------------------------------------------------------- */
/*                          copy constructor                             */
/* --------------------------------------------------------------------- */

VectorTile::VectorTile(const VectorTile& ref)
{
    poTileID_ = new TileID(*ref.poTileID_);
    poLayer_ = ref.poLayer_;
}

/* --------------------------------------------------------------------- */
/*                          operator =                                   */
/* --------------------------------------------------------------------- */

/**
 * Notices for assignment operator
 * ===============================
 * 1] return type reference
 * 2] if(this == &rhs) check
 * 3] clear old memory before allocate new data
 * 4] return *this;
 */

VectorTile& VectorTile::operator= (const VectorTile& ref)
{

    if(this == &ref) return *this; /* effective c++ item 11 */

    clearTile();

    poTileID_ = new TileID(*ref.poTileID_);
    poLayer_ = ref.poLayer_;

    return *this;
}

/* --------------------------------------------------------------------- */
/*                          ～VectorTile()                               */
/* --------------------------------------------------------------------- */

VectorTile::~VectorTile()
{
    clearTile();
}

/* --------------------------------------------------------------------- */
/*                           clearTile()                                 */
/* --------------------------------------------------------------------- */

int VectorTile::clearTile()
{
    if(poTileID_)
    {
        free( poTileID_ );
        poTileID_ = NULL;
    }

    /* should not free features in vector, cause they are managed in layer */

    return 0;
}

/* --------------------------------------------------------------------- */
/*                              getTileID()                              */
/* --------------------------------------------------------------------- */

const TileID* VectorTile::getTileID() const
{
    return poTileID_;
}

/* --------------------------------------------------------------------- */
/*                              getLayer()                               */
/* --------------------------------------------------------------------- */

OGRVTLayer* VectorTile::getLayer() const
{
    return poLayer_;
}

/* --------------------------------------------------------------------- */
/*                           getFeatureCount()                           */
/* --------------------------------------------------------------------- */

int VectorTile::getFeatureCount() const
{
    return papoFeatures_.size();
}


/* --------------------------------------------------------------------- */
/*                             resetReading()                            */
/* --------------------------------------------------------------------- */

/* void VectorTile::resetReading() */
/* { */
/*     nIterator_ = 0; */
/* } */


/* --------------------------------------------------------------------- */
/*                            loadTile()                                 */
/* --------------------------------------------------------------------- */

OGRFeature* createFeatureFromRapidJsonObject(
        OGRFeatureDefn* poFeatureDefn, const Value& obj)
{
    OGRFeature* poRet = new OGRFeature(poFeatureDefn);
    /* traverse json object to populate feature files */
    
    /* handle all non-geometry attributes */
    for(Value::ConstMemberIterator itr = obj.MemberBegin(); itr != obj.MemberEnd(); ++itr)
    {
        if(strcmp(itr->name.GetString(), "coordinate") )
        {
            const char* name = itr->name.GetString();
            OGRFieldType fType = poFeatureDefn->GetFieldDefn(
                    poFeatureDefn->GetFieldIndex(name))->GetType();

            if(fType == OFTString)
            {
                poRet->SetField(name, itr->value.GetString());
            }
            else if(fType == OFTInteger)
            {
                poRet->SetField(name, itr->value.GetInt());
            }
            else if(fType == OFTReal)
            {
                poRet->SetField(name, itr->value.GetDouble());
            }
        }
    }

    /* special handle geometry data */
    const Value& coord = obj["coordinate"];
    double x = coord[0].GetDouble();
    double y = coord[1].GetDouble();
    OGRGeometry* poGeom = new OGRPoint(x, y);
    poRet->SetGeometryDirectly( poGeom );
    printf("%f, %f  ", x, y);
    printf("\n");

    return poRet;
}


int VectorTile::loadTile()
{

    char fullFilePath[500];
    memset(fullFilePath, 0,500);
    strcat(fullFilePath, poLayer_->getDataSource()->getDataDir());
    strcat(fullFilePath, "/");
    strcat(fullFilePath, poTileID_->toString());
    strcat(fullFilePath, ".json");

    std::cout<<"vectortile: tile fullFilePath is :"<<fullFilePath<<std::endl;

    FILE* fp = fopen(fullFilePath, "r");
    char readBuffer[65536];
    FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    Document d;
    d.ParseStream(is);

    const Value& features = d["features"];
    SizeType feature_count = features.Size();
    for(SizeType i = 0; i < feature_count; ++i) 
    {
        const Value& fea = features[i];
        OGRFeature* poFeature = 
            createFeatureFromRapidJsonObject(poLayer_->GetLayerDefn(), fea);
        papoFeatures_.push_back(poFeature);
    }

    fclose(fp);
    return 0;
}

/* --------------------------------------------------------------------- */
/*                              performFilting()                         */
/* --------------------------------------------------------------------- */

/**
 * if no error return 0, else return 1
 */

int VectorTile::performFilting(int bFilteGeom, int bFilteAttr)
{
    int fidFiltingFlag = filteFID();
    int geomFiltingFlag = 1;
    int attrFiltingFlag = 1;

    if(bFilteGeom) { geomFiltingFlag = filteGeometry();}
    if(bFilteAttr) { attrFiltingFlag = filteAttributes();}

    return  !((!fidFiltingFlag) && 
              (bFilteGeom ? (!geomFiltingFlag) : 1) && 
              (bFilteAttr ? (!attrFiltingFlag) : 1) );
}

/* --------------------------------------------------------------------- */
/*                              filteFID()                               */
/* --------------------------------------------------------------------- */

int VectorTile::filteFID()
{
    /* if(bFIDFilted_) return 0; */

    /* /1* if not filted *1/ */
    /* int nMax = papoFeatures_.size(); */
    /* for(int i=0; i<nMax; ++i) */
    /* { */
    /*     if( ( poFeatureCompatibleFlags_[i] && */ 
    /*             poLayer_->GetHash()->has( papoFeatures_[i]->GetFID()) ) || */
    /*             !papoFeatures_[i] ) */
    /*     { */
    /*         poFeatureCompatibleFlags_[i] = 0; */
    /*         nCompatibleFeature_--; */
    /*     } */
    /* } */

    /* bFIDFilted_ = 1; */
    return 0;
}

/* --------------------------------------------------------------------- */
/*                              filteGeometry()                          */
/* --------------------------------------------------------------------- */

int VectorTile::filteGeometry()
{
    /* int nMax = papoFeatures_.size(); */
    /* for(int i=0; i<nMax; ++i) */
    /* { */
    /*     if(poFeatureCompatibleFlags_[i] && papoFeatures_[i] && */ 
    /*             /1* TODO: add geometry filtering judgement *1/ */
    /*             poLayer_->GetHash()->has( papoFeatures_[i]->GetFID()) ) */
    /*     { */
    /*         poFeatureCompatibleFlags_[i] = 0; */
    /*         nCompatibleFeature_--; */
    /*     } */
    /* } */
    return 0;
}

/* --------------------------------------------------------------------- */
/*                              filteAttributes()                        */
/* --------------------------------------------------------------------- */

int VectorTile::filteAttributes()
{
    printf("not implemented yet");
    return 0;
}

/* --------------------------------------------------------------------- */
/*                              commitToLayer()                          */
/* --------------------------------------------------------------------- */

int VectorTile::commitToLayer()

{
    std::vector<OGRFeature*>::iterator itr;
    for(itr = papoFeatures_.begin(); itr!=papoFeatures_.end(); ++itr)
    {
        OGRGeometry* poG = (*itr)->GetGeometryRef();
        if(poLayer_->FilterGeometryWrap( poG ) )
        {
            OGRFeature* poF = (*itr);
            if(poF) 
            {
                poLayer_->addFeatureDirectly(poF);
                printf("vectortile: commit a feature\n");
            }
        }
        else
        {
            delete (*itr);
        }
    }
    return 0;
}

/* --------------------------------------------------------------------- */
/*                              AddFeature()                             */
/* --------------------------------------------------------------------- */

bool VectorTile::AddFeature(OGRFeature* poFeature)

{
    if(!poFeature) return false;

    papoFeatures_.push_back( poFeature->Clone() );
    /* poFeatureCompatibleFlags_.push_back(1); */

    return true;
}


