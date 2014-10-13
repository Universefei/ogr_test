WPS data classification strategy
================================

## Background

* Data: Open Street Map
* DataBase: PostGIS

## Strategy

* Reduce feature by 'type' field.
* Reduce points amount.

### Reduce feature

Reduce feature can be accomplished by SQL stuff through database.

### Reduce points

To reduce points amont in specific layer, aka table in database, access PostGIS and do 
reducation utilizing GDAL/OGR library. 

There are 2 ways to import reduced result to PostGIS:

* Export to GeoJSON or Shapefile via OGR, then import using ogr2ogr.
* Coding a program using libpq to import, can reference osm2pgsql or shp2pgsql source code.
