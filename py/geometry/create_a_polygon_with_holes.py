#!/usr/bin/env python

from osgeo import ogr

extRing = ogr.Geometry(ogr.wkbLinearRing)
innRing = ogr.Geometry(ogr.wkbLinearRing)

extRing.AddPoint(0, 0)
extRing.AddPoint(1, 0)
extRing.AddPoint(1, 1)
extRing.AddPoint(0, 1)
extRing.AddPoint(0, 0)

innRing.AddPoint(0.25, 0,25)
innRing.AddPoint(0.75, 0,25)
innRing.AddPoint(0.75, 0,75)
innRing.AddPoint(0.25, 0,75)
innRing.AddPoint(0.25, 0,25)

# Create polygon
polygonWithHole = ogr.Geometry(ogr.wkbPolygon)
polygonWithHole.AddGeometry(extRing)
polygonWithHole.AddGeometry(innRing)

print 'polygonWithHole:'
print polygonWithHole.ExportToWkt()
