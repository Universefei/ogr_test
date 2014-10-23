#!/usr/bin/env python

from osgeo import ogr

# Create ring
ring = ogr.Geometry(ogr.wkbLinearRing)
ring.AddPoint(1, 2)
ring.AddPoint(2, 2)
ring.AddPoint(3, 2)
ring.AddPoint(4, 2)
ring.AddPoint(5, 2)
ring.AddPoint(6, 2)
ring.AddPoint(7, 8)
ring.AddPoint(1, 2) # last point must identical to the first one
print 'ring:'
print ring.ExportToWkt()

polygon = ogr.Geometry(ogr.wkbPolygon)
polygon.AddGeometry(ring)
print 'polygon:'
print polygon.ExportToWkt()

