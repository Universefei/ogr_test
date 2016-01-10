#!/usr/bin/env bash

# echo $1
g++ -o exe $1 -L/home/fei/proj/ogr_vt/build_mix/lib -lgdal -I/home/fei/proj/ogr_vt/build_mix/include

sudo echo "/home/fei/proj/ogr_vt/build_mix/lib" > /etc/ld.so.conf.d/gdal.conf
sudo ldconfig
./exe
