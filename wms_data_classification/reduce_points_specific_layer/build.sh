#!/usr/bin/env bash

rm waterply_900913 osm_waterareas dc_coastline landusages_lv0 landusages_lv1 osm_landusages osm_mainroads osm_motorways osm_places osm_roads 



g++ -o exe wms_reduce_points.cc -L/usr/local/lib -lgdal

