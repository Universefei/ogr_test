#!/usr/bin/env bash

if [[ -f waterply_900913 ]];
then
    rm waterply_900913
fi

if [[ -f exe ]];
then
    rm exe
fi

g++ -o exe *.cc -L/usr/local/lib -lgdal

