Dump all data in PostGIS into
=============================

## Command
`ogrinfo -al PG:"host='192.168.1.99' port='5432' user='postgres' password='postgres' dbname='opengeo'" >> dumpAllfromPG`

## Note
* You can find out how to orgnize feature data and metadata.
* You can find out what to return in WFS getCapabilities.
* You can find out what to return in WFS describeFeatureType.
