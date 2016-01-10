#include "TileCalculator.h"

#include <cmath>

TileCalculator::TileCalculator()
{

}

TileCalculator::~TileCalculator()
{

}

void TileCalculator::GetTilesExtentOfLatLonMer( LatLonExtent extent, int level, Tile &leftDown, Tile &rightUp )
{
	
}

void TileCalculator::GetExtentOfTileMer( Tile tile, LatLonExtent &pLatLonExtent )
{
	
}

void TileCalculator::GetTilesExtentOfLatLonGeo( LatLonExtent extent, int level, Tile &leftDown, Tile &rightUp )
{
	double latUnit = 360/(pow( 2.0, (double)level )*2.0);
	double lonUnit = 180/pow( 2.0, (double)level );

	double eastRelative = extent.east - ( -180.0 );
	double westRelative = extent.west - ( -180.0 );
	double southRelative = extent.south - ( -90.0 );
	double northRelative = extent.north - (-90);

	/// 选择向下取整
	leftDown.x = westRelative/latUnit;
	leftDown.y = southRelative/lonUnit;
	rightUp.x = eastRelative/latUnit;
	rightUp.y = northRelative/lonUnit;
}

void TileCalculator::GetExtentOfTileGeo( Tile tile, LatLonExtent &pLatLonExtent )
{
	double latUnit = 360/(pow( 2.0, (double)tile.level )*2.0);
	double lonUnit = 180/pow( 2.0, (double)tile.level );

	double eastRelative = ( tile.x + 1 ) * latUnit;
	double westRelative = tile.x * latUnit;
	double southRelative = tile.y * lonUnit;
	double northRelative = ( tile.y + 1 )* lonUnit;

	pLatLonExtent.east = eastRelative + ( -180.0 );
	pLatLonExtent.west = westRelative + ( -180.0 );
	pLatLonExtent.south = southRelative + ( -90.0 );
	pLatLonExtent.north = northRelative + ( -90.0 );
}
