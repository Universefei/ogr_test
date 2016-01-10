#include "ogrsf_frmts.h"
#include <vector>
#include <stdio.h>

#include <iostream>
using namespace std;


void printFeature(OGRFeature* poF)
{
    if(!poF) return;

    std::cout<<"fid:["<<poF->GetFieldAsInteger("fid")<<"] ";
    std::cout<<"name:["<<poF->GetFieldAsString("name")<<"] ";
    OGRPoint* poPoint = dynamic_cast<OGRPoint*>(poF->GetGeometryRef());
    if(poPoint)
    {
        std::cout<<"coordinate:["<<poPoint->getX()<<", "<<poPoint->getY()<<"]";
    }
    std::cout<<std::endl;
}


int main (int argc, char const* argv[])
{
    OGRSFDriver     *poDriver = NULL;
    OGRDataSource   *poDS = NULL;
    OGRLayer        *poLayer = NULL;
    int             nLayers = 0;

    RegisterOGRVT();
    poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName("Vector Tile");
    if(!poDriver)
    {
        fprintf(stderr, "Get Driver Vector Tile error \n");
        exit(1);
    }

    const char*     pszDataDir = "VT: dir=/home/fei/proj/ogr_vt/data";
    try
    {
        poDS = poDriver->Open(pszDataDir);
    }
    catch(...)
    {
        fprintf(stderr,"open datadir datasource error\n");
    }

    printf("what\n");
    printf("fei\n");
    if(!poDS)
    {

        printf("construct datasource error ...\n");
        exit(1);
    };

    nLayers = poDS->GetLayerCount();
    printf("there is (%d) layers\n", nLayers);

    int iLyr = 0;
    char* pszSRSWkt = NULL;

    poLayer = poDS->GetLayerByName("hospital");
    if(poLayer)
    {
        printf("get layer ok!\n");
    }
    OGRPolygon* poPolygon = new OGRPolygon();
    OGRLinearRing* poRing = new OGRLinearRing();
    poRing->addPoint(-120.01, 80.01);
    poRing->addPoint(-120.02, 70.01);
    poRing->addPoint(-100.01, 70.02);
    poRing->addPoint(-90.01, 75.02);
    poRing->addPoint(-100.01, 80.02);
    poRing->addPoint(-120.01, 80.01);
    poRing->closeRings();
    poPolygon->addRingDirectly(poRing);
    if(!poPolygon->IsEmpty())
    {
        printf("poPolygon obj is not empty!\n");
        printf("filter polygon's area is %f\n", poPolygon->get_Area());
    }
    if(!poPolygon->IsValid())
    {
        printf("poPolygon obj is not valid!\n");
    }
    OGRLinearRing* poExtRing = poPolygon->getExteriorRing();
    if(poExtRing)
    {
        int nPoints = poExtRing->getNumPoints();
        for(int i=0; i<nPoints; ++i)
        {
            printf("point %d: x=%f y=%f\n", i, poExtRing->getX(i), poExtRing->getY(i));
        }

    }

    int ninterRings = poPolygon->getNumInteriorRings();
    poLayer->SetSpatialFilter(poPolygon);

    /* poLayer->ResetReading(); // this will be called automatically in SetSpatialFilter */

    if(poLayer)
    {
        printf("============================results=============================\n");
        printf("feature count is %d\n", poLayer->GetFeatureCount());
        printf("\n");
        OGRFeature* poF = poLayer->GetNextFeature();

        while(poF)
        {
            printFeature(poF);
            poF = poLayer->GetNextFeature();
        }
    }
    
    return 0;
}
