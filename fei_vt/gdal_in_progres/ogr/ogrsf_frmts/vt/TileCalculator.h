#ifndef __CALCULATOR_H_
#define __CALCULATOR_H_

struct Tile
{
	int level;
	int x;
	int y;
};

struct LatLonExtent
{
public:
	double east;
	double west;
	double south;
	double north;
	/// double latitude;
	/// double longtitude;
public:
	LatLonExtent( double peast, double pwest, double psouth, double pnorth )
		: east(peast), west(pwest), south(psouth), north(pnorth){}

	LatLonExtent()
		: east(0.0), west(0.0), south(0.0),north(0.0){}
};


class TileCalculator
{
public:
	//////////////////////////////////////////////////////////////////////////
	/// In Degrees
	//////////////////////////////////////////////////////////////////////////
    
	static void GetTilesExtentOfLatLonMer( LatLonExtent extent, int level, 
                                         Tile &leftDown, Tile &rightUp );

	static void GetExtentOfTileMer( Tile tile, LatLonExtent &leftUp );

	static void GetTilesExtentOfLatLonGeo( LatLonExtent extent, int level, 
                                         Tile &leftDown, Tile &rightUp );

	static void GetExtentOfTileGeo( Tile tile, LatLonExtent &leftUp );

	TileCalculator();
	~TileCalculator();
};

#endif
