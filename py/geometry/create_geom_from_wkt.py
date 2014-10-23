#!/usr/bin/env python

from osgeo import ogr

wkt = "POINT(1 5)"
point = ogr.CreateGeometryFromWkt(wkt)

print "%d, %d" % (point.GetX(), point.GetY())
