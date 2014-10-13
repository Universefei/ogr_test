#include "PGData.h"
#include <stdlib.h>
#include <string.h>


/* --------------------------------------------------------------------- */
/*     GetLayerEnum()                                                    */
/* --------------------------------------------------------------------- */

ELayer GetLayerEnum(const char* layerName,int layerLevel)
{
    if(strcmp(layerName, "planet_osm_points") == 0)
        return Eplanet_osm_points;
    else if( strcmp(layerName, "planet_osm_roads") == 0 )
    {
        return Eplanet_osm_roads;
    }
    else if( strcmp(layerName, "planet_osm_waterareas") == 0 )
    {
        return Eplanet_osm_waterareas;
    }
    else if( strcmp(layerName, "planet_osm_landusages") == 0 )
    {
        return Eplanet_osm_landusages;
    }
    else 
        return Enon_layer;
}

/* --------------------------------------------------------------------- */
/*     GetTableName()                                                    */
/* --------------------------------------------------------------------- */

extern const char*** dataLevelTable;

const char* GetTableName(const char* layerName, int layerLevel)
{
    ELayer layerID = GetLayerEnum(layerName, layerLevel);
    if( layerID != Enon_layer )
    {
        return dataLevelTable[layerID][layerLevel];
    }
}
