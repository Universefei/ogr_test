#ifndef PGDATA_H
#define PGDATA_H

enum ELayer{
    Eplanet_osm_points       = 0,
    Eplanet_osm_roads        = 1,
    Eplanet_osm_waterareas   = 2,
    Eplanet_osm_landusages   = 3,
    Enon_layer               = 0xff
}

/* --------------------------------------------------------------------- */
/*     A table to map (layer, level) to table name in PostGIS            */
/* --------------------------------------------------------------------- */
const char dataLevelTable[4][19][] = 
{
    // planet_osm_points.
    {
        /* Level 0 */           "planet_osm_points",
        /* Level 1 */           "planet_osm_points",
        /* Level 2 */           "planet_osm_points",
        /* Level 3 */           "planet_osm_points",
        /* Level 4 */           "planet_osm_points",
        /* Level 5 */           "planet_osm_points",
        /* Level 6 */           "planet_osm_points",
        /* Level 7 */           "planet_osm_points",
        /* Level 8 */           "planet_osm_points",
        /* Level 9 */           "planet_osm_points",
        /* Level 10 */          "planet_osm_points",
        /* Level 11 */          "planet_osm_points",
        /* Level 12 */          "planet_osm_points",
        /* Level 13 */          "planet_osm_points",
        /* Level 14 */          "planet_osm_points",
        /* Level 15 */          "planet_osm_points",
        /* Level 16 */          "planet_osm_points",
        /* Level 17 */          "planet_osm_points",
        /* Level 18 */          "planet_osm_points",
    },
    // planet_osm_roads.
    {
        /* Level 0 */           "planet_osm_roads",
        /* Level 1 */           "planet_osm_roads",
        /* Level 2 */           "planet_osm_roads",
        /* Level 3 */           "planet_osm_roads",
        /* Level 4 */           "planet_osm_roads",
        /* Level 5 */           "planet_osm_roads",
        /* Level 6 */           "planet_osm_roads",
        /* Level 7 */           "planet_osm_roads",
        /* Level 8 */           "planet_osm_roads",
        /* Level 9 */           "planet_osm_roads",
        /* Level 10 */          "planet_osm_roads",
        /* Level 11 */          "planet_osm_roads",
        /* Level 12 */          "planet_osm_roads",
        /* Level 13 */          "planet_osm_roads",
        /* Level 14 */          "planet_osm_roads",
        /* Level 15 */          "planet_osm_roads",
        /* Level 16 */          "planet_osm_roads",
        /* Level 17 */          "planet_osm_roads",
        /* Level 18 */          "planet_osm_roads",

    },
    // planet_osm_waterareas.
    {
        /* Level 0 */           "planet_osm_waterareas",
        /* Level 1 */           "planet_osm_waterareas",
        /* Level 2 */           "planet_osm_waterareas",
        /* Level 3 */           "planet_osm_waterareas",
        /* Level 4 */           "planet_osm_waterareas",
        /* Level 5 */           "planet_osm_waterareas",
        /* Level 6 */           "planet_osm_waterareas",
        /* Level 7 */           "planet_osm_waterareas",
        /* Level 8 */           "planet_osm_waterareas",
        /* Level 9 */           "planet_osm_waterareas",
        /* Level 10 */          "planet_osm_waterareas",
        /* Level 11 */          "planet_osm_waterareas",
        /* Level 12 */          "planet_osm_waterareas",
        /* Level 13 */          "planet_osm_waterareas",
        /* Level 14 */          "planet_osm_waterareas",
        /* Level 15 */          "planet_osm_waterareas",
        /* Level 16 */          "planet_osm_waterareas",
        /* Level 17 */          "planet_osm_waterareas",
        /* Level 18 */          "planet_osm_waterareas",

    },
    // planet_osm_landusage.
    {
        /* Level 0 */           "planet_osm_landusages",
        /* Level 1 */           "planet_osm_landusages",
        /* Level 2 */           "planet_osm_landusages",
        /* Level 3 */           "planet_osm_landusages",
        /* Level 4 */           "planet_osm_landusages",
        /* Level 5 */           "planet_osm_landusages",
        /* Level 6 */           "planet_osm_landusages",
        /* Level 7 */           "landusages_lv0",
        /* Level 8 */           "landusages_lv0",
        /* Level 9 */           "landusages_lv0",
        /* Level 10 */          "landusages_lv0",
        /* Level 11 */          "landusages_lv0",
        /* Level 12 */          "landusages_lv1",
        /* Level 13 */          "landusages_lv1",
        /* Level 14 */          "landusages_lv1",
        /* Level 15 */          "landusages_lv1",
        /* Level 16 */          "landusages_lv1",
        /* Level 17 */          "landusages_lv1",
        /* Level 18 */          "landusages_lv1",
    }
};


const char* GetTableName(const char* layerName, int layerLevel);

#endif // PGDATA_H
