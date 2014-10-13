#include "ogr_spatialref.h"
#include "ogrsf_frmts.h"
#include "ogr_geometry.h"

#include <iostream>
using namespace std;


int main()
{
    OGRRegisterAll();
    //const string srcSRS = "EPSG:900913";
    const string srcSRS = "EPSG:4326";
    const string dstSRS = "EPSG:900913";
    //const string dstSRS = "EPSG:4326";

    OGRSpatialReference* poSourceSRS = (OGRSpatialReference*)OSRNewSpatialReference(NULL);
    poSourceSRS->SetFromUserInput(srcSRS.c_str());
    OGRSpatialReference* poOutputSRS = (OGRSpatialReference*)OSRNewSpatialReference(NULL);
    poOutputSRS->SetFromUserInput(dstSRS.c_str());

    OGRCoordinateTransformation* poCT = OGRCreateCoordinateTransformation(
            poSourceSRS, poOutputSRS);


    OGRPoint* poPoint = new OGRPoint(30.0, 30.0, 60.0);

    OGRGeometry* poReprojectedGeom =
        OGRGeometryFactory::transformWithOptions(poPoint, poCT, NULL);

    OGRPoint* poDstPoint = dynamic_cast<OGRPoint*>(poReprojectedGeom);
    cout<<"X: "<<poDstPoint->getX()<<endl;
    cout<<"Y: "<<poDstPoint->getY()<<endl;
    cout<<"Z: "<<poDstPoint->getZ()<<endl;
    
    return 1;
}

