#!/usr/bin/env bash

for geojson in *_blured_leap_5
do
    ogr2ogr -f 'ESRI Shapefile' ${geojson}_shp ${geojson}
    echo "Converting ${geojson}                             done"
done
