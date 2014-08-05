#!/usr/bin/env bash

osm2pgsql -s -U postgres -P 5432 -d osmgis ~/百度云同步盘/GIS/DataResource/antarctica-latest.osm.pbf
# Do not use -W password here
# If not found default.style file, brew install osm2pgsql on OSX and find that copy of default.style
# \and then cp that under specific path.
