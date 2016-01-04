#!/usr/bin/env python

import yaml
import sys
import random
import string
import json
import os
try: import simplejson as json
except ImportError: import json



##############################################################################
# describe: get geograpihc coordinates of a given tile id
# input: tile id
# output: geographic coordinates of tile
##############################################################################
def get_tile_position(zoom, x, y):
    row_count = 2**(zoom-1)
    column_count = row_count * 2
    left = -180 + (x-1) * (360/column_count)
    up = 90 - (y-1) * (180/row_count)
    return (left, up, left + 360/column_count, up - 180/row_count)


##############################################################################
# describe: generate random string, maximal length is 30
##############################################################################
def random_string():
    return ''.join(random.choice(string.lowercase) for i in range(30))


##############################################################################
# describe: generate a random feature in a given range
# input: range of geographic coodinates and fid
# output: feature object
##############################################################################
def random_point(left_border, up_border, right_border, down_border, fid):
    point = {}
    wide = right_border - left_border
    length = up_border - down_border
    point["coordiante"] = (left_border + random.random()* wide, 
            down_border + random.random()*length)
    point["fid"] = fid
    point["name"] = random_string()
    print "generate point: " + str(point)
    return point


##############################################################################
# input: tile position and random points count to generate in this tile
# output: generated points array in specific tile
##############################################################################
def populate_tile_points(zoom, x, y, amount, start_fid):
    print "populate_tile: " + str(zoom) + "#" + str(x)+ "#" + str(y)
    (left, up, right, down) = get_tile_position(zoom, x, y)
    points_array = []
    for i in range(0, amount):
        generate_random = random_point(left, up, right, down, start_fid + i)
        start_fid += 1
        points_array.append(generate_random)
    return points_array


##############################################################################
# describe: write features in one tile into json file
# input: output file name and feature array
##############################################################################
def write_to_json(dir_path, file_name, feature_array):
    tile = {}
    tile["type"] = "point"
    tile["geometry_column"] = "coordinate"
    tile["features"] = feature_array
    json.dump(tile, open(dir_path.rstrip('/') + '/' + file_name, 'w') )


##############################################################################
# describe: write tile into redis in json format
##############################################################################
def write_to_redis(key_name, feature_array):
    #TODO
    pass


##############################################################################
# describe: write features in one tile into json
# input: layername, zoom level, and totoal point amount to generate
##############################################################################
def generate_tiles(dir_path, layer_name, zoom, point_amount):
    side_len = 2 ** (zoom-1)
    start_fid = 0
    for x in range(1, 2 * side_len + 1 ):
        for y in range(1, side_len + 1):
            tile_point_count = point_amount / ((side_len ** 2) * 2)
            points_array = populate_tile_points(zoom, x, y, tile_point_count, start_fid)
            start_fid += tile_point_count
            tile_key = layer_name + "#" + str(zoom) + "#" + str(x) + "#" + str(y)
            write_to_json(dir_path, tile_key + ".json", points_array)


if __name__ == "__main__":
    config_path = sys.argv[1]
    with open(config_path) as fp:
        config = yaml.load(fp)
    zoom = config["zoom"]
    
    point_amount = config["point_amount"]
    layer_name = config["layer_name"]
    dir_name = config["json_dir_name"]
    cur_dir = os.getcwd()
    dir_path =cur_dir.rstrip('/') + '/' + dir_name.rstrip('/')
    if not os.path.exists(dir_path):
        os.mkdir(dir_path)
    

    generate_tiles(dir_path, layer_name, zoom, point_amount)

