/******************************************************************************
 * $Id: ogrmemlayer.cpp 27044 2014-03-16 23:41:27Z rouault $
 *
 * Project:  OpenGIS Simple Features Reference Implementation
 * Purpose:  Implements OGRVTLayer class.
 * Author:   Frank Warmerdam, warmerdam@pobox.com
 *
 ****************************************************************************/

#include "ogr_vt.h"
#include "cpl_conv.h"
#include "ogr_p.h"
#include "ogr_geometry.h"
#include <iostream>
#include <cstdlib>
#include <vector>
#include "TileCalculator.h"


/************************************************************************/
/*                           PerformFilter()                            */
/************************************************************************/

void OGRVTLayer::PerformFilter()

{
    /**
     * work flow
     * ====================================
     * Three steps:
     * 1]  find out tile numbers through geometry filter.
     * 2]  fetch tiles through tile number one by one.
     * 3]  secondary precious filting. (class Vector Tile or mannually handle)
     * 4]  add authorized feature into layer.
     * 5]  update filterModified flag.
     */

    /* if(bFilterModified) */
    /* { */
    /*     TileIDSet   tileIds; */
    /*     TileIDSet::iterator itTiles; */
    /*     tileIds = GetIntersectTiles(GetSpatialFilter()); */

    /*     VectorTile* poVT = new GeoJSONVectorTile(this); */

    /*     for(itTiles=tileIds.begin(); itTiles!=tileIds.end(); itTiles++) */
    /*     { */
    /*         /1* fetch and do filtering *1/ */
    /*         poVT->fetchTile( *itTiles ); */
    /*         poVT->performFilting(); */
    /*         poVT->commitToLayer(); */
    /*         poVT->clearTile(); /1* fei: release tile occupied resource *1/ */
    /*     } */

    /*     delete poVT; */
    /* } */

    /* bFilterModified = 0; */
}


/************************************************************************/
/*                              GetTile()                               */
/************************************************************************/

int OGRVTLayer::GetTile(int x, int y, int z)

{
    TileID tmpTile(GetName(), x, y, z);
    return GetTile(&tmpTile);
}

int OGRVTLayer::GetTile(TileID* poTileID)

{
    /* TODO: check if error occured, may connection failed */
    /* VectorTile* poVTile = new GeoJSONVectorTile(this); */
    /* poVTile->fetchTile(poTileID); */
    /* poVTile->performFilting(); */
    /* poVTile->commitToLayer(); */
    /* delete poVTile; /1* TODO; may add to Vecter cache *1/ */

    return 0;
}


/************************************************************************/
/*                            ResetReading()                            */
/************************************************************************/

void OGRVTLayer::ResetReading()

{
    iNextReadFID = 0;
    std::vector<TileID> *poIntersectTID = NULL;
    // get intersected tileids
    if(m_poFilterGeom != NULL)
    {
        poIntersectTID = GetIntersectTIDs(m_poFilterGeom, zoom);
    }
    else // load all tiles
    {
        poIntersectTID = GetAllTIDs(zoom);
    }

    for(std::vector<TileID>::iterator itr = poIntersectTID->begin();
            itr != poIntersectTID->end(); ++itr)
    {
        /* printf("load tileid: %s\n", itr->toString()); */
        std::cout<<"VTLayer_add: load tileid: "<<itr->toString()<<std::endl;
        VectorTile* poVT = new VectorTile(this, itr->z_, itr->x_, itr->y_);
        poVT->loadTile();
        poVT->commitToLayer();
        delete poVT;
    }

    if(poIntersectTID)
    {
        delete poIntersectTID;
    }
}


/************************************************************************/
/*                           GetIntersectTIDs()                        */
/************************************************************************/

/* vector<TileID*> OGRVTLayer::GetIntersectTiles(double minx, double miny, */ 
/*                             double maxx, double maxy, int srid, int level) */
/* { */
/*     /1* OGRLinearString* polinear = new OGRLinearString(); *1/ */
/*     OGRLinearString polinear(); */
/*     polinear->addPoint(minx, miny); */
/*     polinear->addPoint(minx, maxy); */
/*     polinear->addPoint(maxx, maxy); */
/*     polinear->addPoint(maxx, miny); */
/*     polinear->addPoint(minx, miny); */


/*     /1* OGRGeometry* poExtent =  new OGRPolygon(); *1/ */
/*     OGRGeometry poExtent(); */
/*     poExtent->addRingDirectly(polinear); */
    
/*     return GetIntersectTiles(poExtent, level); */
/* } */

/************************************************************************/
/*                            GetAllTIDs()                              */
/************************************************************************/

#include "math.h"
std::vector<TileID> *OGRVTLayer::GetAllTIDs(int zoom)

{
    std::vector<TileID>* poTIDs = new std::vector<TileID>;
    int max_x = pow(2, zoom);
    int max_y = pow(2, zoom - 1);
    for(int x = 1; x <= max_x; ++x)
    {
        for(int y = 1; y <= max_y; ++y)
        {
            poTIDs->push_back(TileID(GetName(), zoom, x, y ));
        }
    }
    return poTIDs;
}

/************************************************************************/
/*                            GetIntersectTiles()                       */
/************************************************************************/

std::vector<TileID> *OGRVTLayer::GetIntersectTIDs(OGRGeometry* poGeom, int level)

{

    return GetIntersectTiles_row(poGeom, level);
}


//////////////////////////////////////////////////////////////////////////
/// 该函数具体属于哪个类？是不是不属于HadoopKV
/// 该函数默认知道：所在图层，所在图层坐标系，所在图层名等
//////////////////////////////////////////////////////////////////////////
#include <iostream>

std::vector<TileID> *OGRVTLayer::GetIntersectTiles_row( OGRGeometry* poGeom, int level )
{
	std::vector<TileID> *poRet = new std::vector<TileID>;
    /* printf("VT: should be ok here!\n"); */

    OGREnvelope *ogrEnvelope = new OGREnvelope;
    poGeom->getEnvelope( ogrEnvelope );
    
    int minTileX = (ogrEnvelope->MinX + 180.0)/(360.0/pow(2, level)) + 1;
    int maxTileX = (ogrEnvelope->MaxX + 180.0)/(360.0/pow(2, level)) + 1;
    int minTileY = (90.0 - ogrEnvelope->MaxY)/(180.0/pow(2, level - 1)) + 1;
    int maxTileY = (90.0 - ogrEnvelope->MinY)/(180.0/pow(2, level - 1)) + 1;
    std::cout<<"tileX,Y min, max"<<minTileX<<","<<maxTileX<<" "<<minTileY<<", "<<maxTileY<<std::endl;
    std::cout<<"-----------------result tileids-------------"<<std::endl;
    for(int x = minTileX; x<=maxTileX; ++x)
    {
        for(int y = minTileY; y<=maxTileY; ++y)
        {
            
            std::cout<<x<<", "<<y<<"  ";
            poRet->push_back(TileID(GetName(), level, x, y));
        }
    }
    std::cout<<std::endl;

	return poRet;
}


/************************************************************************/
/*                            FilterGeometryWrap()                      */
/************************************************************************/

int OGRVTLayer::FilterGeometryWrap(OGRGeometry* poToCheck)
{
    return FilterGeometry(poToCheck);
}

/************************************************************************/
/*                            addFeatureDirectly()                      */
/************************************************************************/

int OGRVTLayer::addFeatureDirectly(OGRFeature* poF)
{
    if(NULL != poF)
    {
        m_vFeatures.push_back(poF);
        nMaxFeatureCount++;

        return 1;
    }
    return 0;

}
