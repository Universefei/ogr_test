#ifndef __OGRVTLAYER_H__
#define __OGRVTLAYER_H__

#include "ogrsf_frmts.h"
#include "vectortile.h"
#include <vector>
#include "tileid.h"

class OGRVTLayer : public OGRLayer
{
    OGRFeatureDefn      *poFeatureDefn;
    
    int                 nFeatureCount;
    int                 nMaxFeatureCount;
    OGRFeature          **papoFeatures;

    int                 iNextReadFID;
    int                 iNextCreateFID;

    int                 bUpdatable;
    int                 bAdvertizeUTF8;

    int                 bHasHoles;


/* --------------------------------------------------------------------- */
/*                          Added properties                             */
/* --------------------------------------------------------------------- */

  private: /* by fei */

    /* XXX: VT caches should put into datasource or layer? */
    /* VectorTile**        poTiles; /1*  *1/ */
    std::vector<OGRFeature*> m_vFeatures;
    int                 nTiles;
    int                 zoom;

    /* HashTable           *poHash; /1* fei: hashSet to findout duplicated fid *1/ */
    OGRVTDataSource     *poDS; /* fei */

    int                 bFilterModified; /* fei */
    OGRGeometry         *poBBox; /* fei */
    std::vector<TileID>* GetIntersectTiles_row(OGRGeometry* geoFilter, int zoom);

  public: /* by fei */

    /* fei: fetch all feature into layer according to filter */
    void                PerformFilter();  /* fei */
    /* HashTable*          GetHash() const; /1* fei *1/ */

    typedef std::vector<TileID*> TileIDSet;
    std::vector<TileID>*     GetIntersectTIDs(OGRGeometry* poGeom, int level=14);
    std::vector<TileID>*     GetAllTIDs(int zoom);

    /* XXX: get all data into papoFeatures */
    int                 GetTile(int x, int y, int z=14); 
    int                 GetTile(TileID*);
    OGRVTDataSource*    getDataSource() { return poDS;};
    int                 setDataSource(OGRVTDataSource* pds){ poDS = pds; };

    int                 FilterGeometryWrap(OGRGeometry* poToCheck);
    int                 addFeatureDirectly(OGRFeature* poFeature);
    


    /* const KVStore*      GetKVStore() const; /1* needed by class VectorTile *1/ */

/*---------------------------------------------------------------------------*/

  public:
                        OGRVTLayer( const char * pszName,
                                     OGRSpatialReference *poSRS,
                                     OGRwkbGeometryType eGeomType );
                        ~OGRVTLayer();


    void                ResetReading();
    OGRFeature *        GetNextFeature();
    virtual OGRErr      SetNextByIndex( long nIndex );

    OGRFeature         *GetFeature( long nFeatureId );
    OGRErr              SetFeature( OGRFeature *poFeature );
    OGRErr              CreateFeature( OGRFeature *poFeature );
    virtual OGRErr      DeleteFeature( long nFID );
    
    OGRFeatureDefn *    GetLayerDefn() { return poFeatureDefn; }

    int                 GetFeatureCount( int );

    virtual OGRErr      CreateField( OGRFieldDefn *poField,
                                     int bApproxOK = TRUE );
    virtual OGRErr      DeleteField( int iField );
    virtual OGRErr      ReorderFields( int* panMap );
    virtual OGRErr      AlterFieldDefn( int iField, OGRFieldDefn* poNewFieldDefn, int nFlags );
    virtual OGRErr      CreateGeomField( OGRGeomFieldDefn *poGeomField,
                                         int bApproxOK = TRUE );

    int                 TestCapability( const char * );

    void                SetUpdatable(int bUpdatableIn) { bUpdatable = bUpdatableIn; }
    void                SetAdvertizeUTF8(int bAdvertizeUTF8In) { bAdvertizeUTF8 = bAdvertizeUTF8In; }

    int                 GetNextReadFID() { return iNextReadFID; }

};

#endif
