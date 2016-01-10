#include <iostream>
#include "ogrsf_frmts.h"
using namespace std;

void PrintFeature(OGRFeature* poFeature) {

}

int main (int argc, char const* argv[])
{
    OGRRegisterAll();
    RegisterOGRVT();
    RegisterOGRMEM();
    cout<<""<<endl;


    /******************************************************************************
     *   Open DataSource and Layer
     *****************************************************************************/
    
    /* OGRSFDriver *poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName("Vector Tile"); */
    /* if(!poDriver) cout<<"get driver error!"<<endl; else cout<<"get driver OK!"<<endl; */
    /* OGRDataSource* poDS= poDriver->open("VT: datasource=file path=/"); */
    /* if(!poDS) cout<<"get datasource error!"<<endl; else cout<<"get datasource OK!"<<endl; */
    /* OGRLayer* poLayer = poDS->getLayerByName("hospital"); */
    /* if(!poLayer) cout<<"get layer error!"<<endl; else cout<<"get layer OK!"<<endl; */


    /******************************************************************************
     *   Set Filter
     *****************************************************************************/
    
    /* OGRGeometry* poSpatialFilter = new OGRPolygon(); */
    /* poSpatialFilter->addRing() */

    /* poLayer->setSpatialFilter(); */
    /* poLayer->resetReading(); */


    /******************************************************************************
     *   Get Result
     *****************************************************************************/
    
    /* OGRFeature *poFeature = poLayer->GetNextFeature(); */
    /* while(poFeature) { */

    /*     // TODO */
    /*     printFeature(poFeature); */
    /*     poFeature->destroy(); */
    /* } */
    

    return 0;
}
