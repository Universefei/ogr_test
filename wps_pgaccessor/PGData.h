#ifndef PGDATA_H
#define PGDATA_H

enum ELayer{
    Eosm_points       = 0,
    Eosm_roads        = 1,
    Eosm_waterareas   = 2,
    Eosm_landusages   = 3,
    Enon_layer        = 0xff
}

extern const char*** dataLevelTable;

const char* GetTableName(const char* layerName, int layerLevel);

#endif // PGDATA_H
