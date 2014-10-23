#!/usr/bin/env python

from osgeo import ogr

# Create wkbLineString to store data
line = ogr.Geometry(ogr.wkbLineString)
line.AddPoint(1,1)
line.AddPoint(1,4)
line.AddPoint(2,1)
line.AddPoint(2,5)
line.AddPoint(3,4)
line.AddPoint(6,9)

print line.ExportToWkt()

