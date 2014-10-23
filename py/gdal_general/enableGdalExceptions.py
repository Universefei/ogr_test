#!/usr/bin/env python

from osgeo import gdal

# enable GDAL/OGR exceptions
# gdal.UseExceptions()

# disable GDAL/OGR exceptions
gdal.DontUseExceptions()

ds = gdal.Open('test.tif')
