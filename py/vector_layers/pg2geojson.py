#!/usr/bin/env python

from osgeo import ogr
import os

host        = "192.168.1.99"
port        = "5432"
user        = "postgres"
password    = "postgres"
dbname      = "dc"
## ========================================================================= ##
#           connect to PG
## ========================================================================= ##

# PGDriverName = "PostgreSQL"
outDriverName = "GeoJSON"
outDriver = ogr.GetDriverByName(outDriverName)

driver = ogr.GetDriverByName("PostgreSQL")
if driver is None:
    sys.exit("NO Driver")

pgConnInfo = "PG: host='%s' port='%s' user='%s' password='%s' dbname='%s'" %\
        (host, port, user, password, dbname)
try:
    pgDS = ogr.Open(pgConnInfo)
except:
    sys.exit("Error: open PostGIS error")

## ========================================================================= ##
#           Iterate each layer in a DataSource
## ========================================================================= ##
# layerNamesList = []
# for iLayer in pgDS:
#     daLayer = iLayer.GetName()
#     if not daLayer in layerNamesList:
#         layerNamesList.append(daLayer)

#     print "layer name: %s" % (iLayer.GetName())
#     print "feature amount: %d" % (iLayer.GetFeatureCount())
#     for feature in iLayer:
#         gid = feature.GetField("geom ")

# layerNamesList.sort()

# for j in layerNamesList:
#     print j


## ========================================================================= ##
#           Get specific Layer by name
## ========================================================================= ##
attrList = []
layer = pgDS.GetLayer( "osm_places" )
if layer is None:
    print "got layer is None"

layerDefn = layer.GetLayerDefn()
for iattr in range(layerDefn.GetFieldCount()):
    attrList.append(layerDefn.GetFieldDefn(iattr).GetName())

for feature in layer:
    print feature.GetField("type")
    # print feature.GetField("population")
    print feature.GetField("name")
    # print feature.GetField(0)


pgDS.Destroy()




