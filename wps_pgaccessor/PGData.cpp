#include "PGData.h"
#include <stdlib.h>
#include <string.h>

/* --------------------------------------------------------------------- */
/*     A table to map (layer, level) to table name in PostGIS            */
/* --------------------------------------------------------------------- */

const char* dataLevelTable[4][19] = 
{
    // osm_points.
    {
        /* Level 0 */           "osm_points",
        /* Level 1 */           "osm_points",
        /* Level 2 */           "osm_points",
        /* Level 3 */           "osm_points",
        /* Level 4 */           "osm_points",
        /* Level 5 */           "osm_points",
        /* Level 6 */           "osm_points",
        /* Level 7 */           "osm_points",
        /* Level 8 */           "osm_points",
        /* Level 9 */           "osm_points",
        /* Level 10 */          "osm_points",
        /* Level 11 */          "osm_points",
        /* Level 12 */          "osm_points",
        /* Level 13 */          "osm_points",
        /* Level 14 */          "osm_points",
        /* Level 15 */          "osm_points",
        /* Level 16 */          "osm_points",
        /* Level 17 */          "osm_points",
        /* Level 18 */          "osm_points"
    },
    // osm_roads.
    {
        /* Level 0 */           "osm_roads_lv14_blured_5",
        /* Level 1 */           "osm_roads_lv14_blured_5",
        /* Level 2 */           "osm_roads_lv14_blured_5",
        /* Level 3 */           "osm_roads_lv14_blured_5",
        /* Level 4 */           "osm_roads_lv14_blured_5",
        /* Level 5 */           "osm_roads_lv14_blured_5",
        /* Level 6 */           "osm_roads_lv14_blured_5",
        /* Level 7 */           "osm_roads_lv14_blured_5",
        /* Level 8 */           "osm_roads_lv14_blured_5",
        /* Level 9 */           "osm_roads_lv14_blured_5",
        /* Level 10 */          "osm_roads_lv14_blured_5",
        /* Level 11 */          "osm_roads_lv14_blured_5",
        /* Level 12 */          "osm_roads_lv14_blured_5",
        /* Level 13 */          "osm_roads_lv14_blured_5",
        /* Level 14 */          "osm_roads_lv14_blured_5",
        /* Level 15 */          "osm_roads_lv14_blured_5",
        /* Level 16 */          "osm_roads",
        /* Level 17 */          "osm_roads",
        /* Level 18 */          "osm_roads"

    },
    // osm_waterareas.
    {
        /* Level 0 */           "osm_waterareas_blured_5",
        /* Level 1 */           "osm_waterareas_blured_5",
        /* Level 2 */           "osm_waterareas_blured_5",
        /* Level 3 */           "osm_waterareas_blured_5",
        /* Level 4 */           "osm_waterareas_blured_5",
        /* Level 5 */           "osm_waterareas_blured_5",
        /* Level 6 */           "osm_waterareas_blured_5",
        /* Level 7 */           "osm_waterareas_blured_5",
        /* Level 8 */           "osm_waterareas_blured_5",
        /* Level 9 */           "osm_waterareas_blured_5",
        /* Level 10 */          "osm_waterareas_blured_5",
        /* Level 11 */          "osm_waterareas_blured_5",
        /* Level 12 */          "osm_waterareas",
        /* Level 13 */          "osm_waterareas",
        /* Level 14 */          "osm_waterareas",
        /* Level 15 */          "osm_waterareas",
        /* Level 16 */          "osm_waterareas",
        /* Level 17 */          "osm_waterareas",
        /* Level 18 */          "osm_waterareas"

    },
    // osm_landusage.
    {
        /* Level 0 */           "landusages_lv0_blured_5",
        /* Level 1 */           "landusages_lv0_blured_5",
        /* Level 2 */           "landusages_lv0_blured_5",
        /* Level 3 */           "landusages_lv0_blured_5",
        /* Level 4 */           "landusages_lv0_blured_5",
        /* Level 5 */           "landusages_lv0_blured_5",
        /* Level 6 */           "landusages_lv0_blured_5",
        /* Level 7 */           "landusages_lv0_blured_5",
        /* Level 8 */           "landusages_lv0_blured_5",
        /* Level 9 */           "landusages_lv0_blured_5",
        /* Level 10 */          "landusages_lv0_blured_5",
        /* Level 11 */          "landusages_lv0_blured_5",
        /* Level 12 */          "landusages_lv1",
        /* Level 13 */          "landusages_lv1",
        /* Level 14 */          "landusages_lv1",
        /* Level 15 */          "landusages_lv1",
        /* Level 16 */          "landusages_lv1",
        /* Level 17 */          "landusages_lv1",
        /* Level 18 */          "landusages_lv1"
    }
};

/* --------------------------------------------------------------------- */
/*     GetLayerEnum()                                                    */
/* --------------------------------------------------------------------- */

ELayer GetLayerEnum(const char* layerName,int layerLevel)
{
    if(strcmp(layerName, "osm_points") == 0)
        return Eosm_points;
    else if( strcmp(layerName, "osm_roads") == 0 )
    {
        return Eosm_roads;
    }
    else if( strcmp(layerName, "osm_waterareas") == 0 )
    {
        return Eosm_waterareas;
    }
    else if( strcmp(layerName, "osm_landusages") == 0 )
    {
        return Eosm_landusages;
    }
    else 
        return Enon_layer;
}

/* --------------------------------------------------------------------- */
/*     GetTableName()                                                    */
/* --------------------------------------------------------------------- */

const char* GetTableName(const char* layerName, int layerLevel)
{
    ELayer layerID = GetLayerEnum(layerName, layerLevel);
    if( layerID != Enon_layer )
    {
        return dataLevelTable[layerID][layerLevel];
    }
}
