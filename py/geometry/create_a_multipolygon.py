#!/usr/bin/env python

from osgeo import ogr

# Create Polygon1
ring = ogr.Geometry(ogr.wkbLinearRing)
ring.AddPoint(1, 2)
ring.AddPoint(2, 2)
ring.AddPoint(3, 4)
ring.AddPoint(6, 2)
ring.AddPoint(7, 8)
ring.AddPoint(1, 2) # last point must identical to the first one

polygon1 = ogr.Geometry(ogr.wkbPolygon)
polygon1.AddGeometry(ring)

# Create Polygon2
ring = ogr.Geometry(ogr.wkbLinearRing)
ring.AddPoint(3, 2)
ring.AddPoint(4, 6)
ring.AddPoint(5, 2)
ring.AddPoint(3, 2) # last point must identical to the first one

polygon2 = ogr.Geometry(ogr.wkbPolygon)
polygon2.AddGeometry(ring)


# Create MultiPolygon
multiPolygon = ogr.Geometry(ogr.wkbMultiPolygon)
multiPolygon.AddGeometry(polygon1)
multiPolygon.AddGeometry(polygon2)

print multiPolygon.ExportToWkt()
print 'multiPolygon has %d geometries' % (multiPolygon.GetGeometryCount())
print 'multiPolygon has %d points' % (multiPolygon.GetPointCount())

for i in range(0, multiPolygon.GetGeometryCount()):

    # Get a Polygon
    g = multiPolygon.GetGeometryRef(i)
    print "%i), %s" % (i, g.ExportToWkt())
    print "   has %d points" % (g.GetPointCount())

    # Get Exterior LinearRing
    extRing = g.getExteriorRing()
    print '--extRing: %s' % (extRing.ExportToWkt())
    for ipoint in range(0, extRing.GetPointCount()):
        point = extRing.GetPoint(ipoint)
        print "----%i th point: %d %d" % (ipoint, point.GetX(), point.GetY() )

    # Get every interior LinearRing
    for iInRing in range(0, g.getNumInteriroRings()):
        inRing = g.getInteriorRing(iInRing)
        print '--inRing: %s' % (inRing.ExportToWkt())
        for ipoint in range(0, inRing.GetPointCount()):
            point = inRing.GetPoint(ipoint)
            print "----%i th point: %d %d" % (ipoint, point.GetX(), point.GetY() )

    # what's different of %i and %d
    # print "%d), %s" % (i, g.ExportToWkt())
