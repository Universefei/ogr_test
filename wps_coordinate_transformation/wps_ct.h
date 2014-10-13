#ifndef WPS_CT_H
#define WPS_CT_H

#include <ogrsf_frmts.h>
#include <ogr_spatialref.h>

OGRLayer* TransformLayer( OGRLayer* poSrcLayer,
                          OGRLayer* poCTedLayer,
                          OGRCoordinateTransformation* poCT = NULL);

int transformate( const std::string &original_coordinate, 
                  const std::string &target_coordinate, 
                  const std::string &original_file, 
                  const std::string &target_file);
#endif
