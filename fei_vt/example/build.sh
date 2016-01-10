#!/usr/bin/env bash

# echo $1
g++ -o exe $1 -L/home/fei/proj/ogr_vt/build/lib -lgdal -I/home/fei/proj/ogr_vt/build/include

sudo echo "/home/fei/proj/ogr_vt/build/lib" > /etc/ld.so.conf.d/gdal.conf
sudo ldconfig
./exe
