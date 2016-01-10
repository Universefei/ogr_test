#!/usr/bin/env bash

g++ -o exe $1 -L/home/fei/proj/ogr_vt/ok_build/lib -lgdal -I/home/fei/proj/ogr_vt/ok_build/include
# g++ -o exe  -L/usr/local/lib -lgdal -I/usr/local/include $1

sudo echo "/home/fei/proj/ogr_vt/ok_build/lib" > /etc/ld.so.conf.d/gdal.conf
sudo ldconfig
./exe

