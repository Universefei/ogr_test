#!/usr/bin/env python

from osgeo import ogr

# Create 3 linestrings
line1 = ogr.Geometry(ogr.wkbLineString)
line1.AddPoint(1,1)
line1.AddPoint(1,4)
line1.AddPoint(2,1)
line1.AddPoint(2,5)
line1.AddPoint(3,4)
line1.AddPoint(6,9)

line2 = ogr.Geometry(ogr.wkbLineString)
line2.AddPoint(1,1)
line2.AddPoint(1,4)
line2.AddPoint(2,1)
line2.AddPoint(9,9)

line3 = ogr.Geometry(ogr.wkbLineString)
line3.AddPoint(2,5)
line3.AddPoint(3,4)
line3.AddPoint(6,9)
line3.AddPoint(10,2)

# Create a multiLineString to contain up 3
multiLineString = ogr.Geometry(ogr.wkbMultiLineString)
multiLineString.AddGeometry(line1)
multiLineString.AddGeometry(line2)
multiLineString.AddGeometry(line3)

print 'multiLineString:'
print multiLineString.ExportToWkt()
