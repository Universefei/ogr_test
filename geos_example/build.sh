#!/usr/bin/env bash

# g++ -o exe -L/usr/local/lib -lgeos -I/usr/local/include example.cpp
g++ -o exe -L/usr/local/lib -lgeos -I/usr/local/include example.cpp /usr/local/lib/libgeos.so

