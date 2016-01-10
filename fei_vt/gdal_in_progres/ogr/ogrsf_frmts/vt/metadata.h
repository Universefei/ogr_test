#ifndef __METADATA_H__
#define __METADATA_H__

/* #include "ogrvtlayer.h" */
#include <string>
#include <vector>

class OGRVTLayer;

OGRVTLayer* generateLayer( const char* layer_meta_path);

std::vector<std::string> *getLayerList( const char* list_path);

#endif
