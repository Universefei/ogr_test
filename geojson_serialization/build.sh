#!/usr/bin/env bash

g++ -g -o executable *.cpp -L/usr/local/lib -lgdal -ljson-c  \
                -I/usr/local/include/json-c 

