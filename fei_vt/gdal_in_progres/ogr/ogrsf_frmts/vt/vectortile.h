#ifndef __VECTOR_TILE_H__
#define __VECTOR_TILE_H__

#include "ogrsf_frmts.h"
#include <vector>
#include "ogr_vt.h"
class OGRVTLayer;

class VectorTile 
{
protected:

    TileID*             poTileID_;
    OGRVTLayer*         poLayer_;

    std::vector<OGRFeature*> papoFeatures_;

public:
                        VectorTile(OGRVTLayer* poLayer, TileID* poTileID = NULL); 
                        VectorTile(OGRVTLayer* poLayer, int z, int x, int y);
                        VectorTile(const VectorTile& ref_copy_constructor); 
    virtual             ~VectorTile(); 
    VectorTile&         operator= (const VectorTile& assign_constructor); 
    /* VectorTile*         clone(); */ 

    const TileID*       getTileID() const; 
    OGRVTLayer*         getLayer() const; 
    virtual int         getFeatureCount() const; 

    int                 loadTile();

    virtual int         clearTile(); /* XXX: purge all tile data */

    virtual int         commitToLayer() ; /* XXX:commit all compatible feature into poLayer */
    int                 performFilting(int bFilteGeom=0, int bFilteAttr=0); /* XXX: template method pattern */


public:
    bool                AddFeature(OGRFeature* poFeature); /* need by geojson reader */

protected:

    virtual int         filteFID(); /* XXX */
    virtual int         filteGeometry();
    virtual int         filteAttributes();

};


#endif
