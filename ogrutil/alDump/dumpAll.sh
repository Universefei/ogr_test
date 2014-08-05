#!/usr/bin/env bash

ogrinfo -al PG:"host='192.168.1.99' port='5432' user='postgres' password='postgres' dbname='opengeo'" >> dumpAllfromPG
