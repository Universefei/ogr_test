#!/usr/bin/env bash

BUILD=`pwd`
cd $BUILD/../gdal_in_progres
./configure --prefix=$BUILD
make
make install
