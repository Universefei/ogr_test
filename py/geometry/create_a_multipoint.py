#!/usr/bin/env python

from osgeo import ogr

# Create points
point1 = ogr.Geometry(ogr.wkbPoint)
point1.AddPoint(1, 1)

point2 = ogr.Geometry(ogr.wkbPoint)
point2.AddPoint(1, 1)

point3 = ogr.Geometry(ogr.wkbPoint)
point3.AddPoint(1, 1)

point4 = ogr.Geometry(ogr.wkbPoint)
point4.AddPoint(1, 1)

# Create multipoint to contain points
multipoints = ogr.Geometry(ogr.wkbMultiPoint)
multipoints.AddGeometry(point1)
multipoints.AddGeometry(point2)
multipoints.AddGeometry(point3)
multipoints.AddGeometry(point4)

print 'multipoints:'
print multipoints.ExportToWkt()
