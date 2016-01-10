#!/usr/bin/env bash

# echo $1
g++ -o exe $1 -L/home/fei/proj/ogr_vt/build_progres/lib -lgdal -I/home/fei/proj/ogr_vt/build_progres/include

sudo echo "/home/fei/proj/ogr_vt/build_progres/lib" > /etc/ld.so.conf.d/gdal.conf
sudo ldconfig
./exe
