#ifndef __TILEID_H__
#define __TILEID_H__

class BBox;

class TileID
{

public:
                TileID(const char* pszLayerName, int x, int y, int z=0);
                TileID(const TileID& cpy); /* TODO */
                /* TileID(const char*  pszKey); */
                ~TileID();
    TileID&     operator= (const TileID& duplica); /* TODO */
    bool        operator==(const TileID& duplica);

    int         x_; // column number
    int         y_; // row number
    int         z_; // zoom level
    char*       pszLayerName_;  // layerName

    int         setLayerName(const char* pszLayerName);

    // eg: water#10#25#14 
    const char* toString() const; // return key of tile id
    /* int         setFromString(const char* pszKey); // set from key into TileID object */

    // geographic stuff
    BBox*       getGeographBBox();
};

class BBox
{
public:
    BBox(double left, double up, double right, double down);
    ~BBox();
    double      left;
    double      up;
    double      right;
    double      down;
};

#endif
