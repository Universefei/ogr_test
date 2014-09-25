#ifndef WPS_CT_H
#define WPS_CT_H

#include <ogrsf_frmts.h>
#include <ogr_spatialref.h>

int TransformLayer( OGRLayer* poSrcLayer,
                          int iGeomField = 0,
                          OGRCoordinateTransformation* poCT = NULL);

int transformate( const std::string &original_coordinate, 
                  const std::string &target_coordinate, 
                  const std::string &original_file, 
                  const std::string &target_file);
#endif
