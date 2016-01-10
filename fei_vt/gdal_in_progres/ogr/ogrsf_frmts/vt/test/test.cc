#include <iostream>
#include "ogrsf_frmts.h"
using namespace std;


void printField(OGRFeature* poFeature, int i ) {
    OGRFieldDefn *poFieldDefn = poFeature->GetFieldDefnRef(i);

    if(!poFieldDefn) return;

    cout<<poFieldDefn->GetNameRef()<<": ";

    switch(poFieldDefn) {
        case OFTInteger:
            cout<<poFeature->GetValueAsInteger(i)<<endl;
            break;
        case OFTReal:
            cout<<poFeature->GetValueAsReal(i)<<endl;
            break;
        case OFTString:
            cout<<poFeature->GetValueAsString(i)<<endl;
            break;
        case OFTWideString:
            cout<<poFeature->GetValueAsString(i)<<endl;
            break;
        case OFTBinary:
            cout<<poFeature->GetValueAsString(i)<<endl;
            break;
        default:
            cout<<" "<<endl;
    }
}


void printGeometry(OGRFeature* poFeature, int i=0) {
    OGRGeomFieldDefn *poGeomDefn = poFeature->GetGeomFieldDefn(i);
    
    if(!poGeomDefn) return;

    cout<<poGeomDefn->GetNameRef()<<": ";

    switch(poGeomDefn->eGeomType) {
        case wkbPoint:
        case wkbLineString:
        case wkbPolygon:
        case wkbMultiPoint:
        case wkbMultiLineString:
        case wkbMultiPolygon:
        case wkbGeometryCollection:
        default:
    }
}


void testCase1();
void testCase2();


int main() {

    testCase1();
    testCase2();

    return 0;
}

void testCase1() {

    cout<<"--------------------- case 1 ---------------------"<<endl;
    double minx, miny, maxx, maxy;
    RegisterOGRVT();

    // 
    // Connect DataSource and Layer
    //
    OGRSFDriver   *poDriver = 
        OGRSFDriverRegister::GetRegistrar()->GetDriverByName("Vector tile");

    OGRDataSource *poVTDS = poDriver->Open("VT: kv_store=redis host=192.168.1.166");

    //
    // Set Filters and prepare data
    //
    OGRLayer *poLayer = poVTDS->GetLayerByName("buildings");
    poLayer->SetSpatialFilterRect(0, minx, miny, maxx, maxy);
    poLayer->SetAttributeFilter();

    //
    // Read Features
    //
    poLayer->ResetReading();
    
    OGRFeatureDefn *poFeatureDefn= poLayer->GetLayerDefn();
    OGRFeature *poFeature = poLayer->GetNextFeature();

    while(poFeature) {
        int fieldCnt = poFeatureDefn->GetFieldCount();
        for(int i = 0; i < fieldCnt; ++i) {
            printField(poFeature, i);
        }
        
        int geomCnt = poFeatureDefn->GetGeomFieldCount();
        for(int i = 0; i < geomCnt; ++i) {
            printGeometry(poFeature, i);
        }

        cout<<endl;
    }
}


void testCase2() {

    cout<<"--------------------- case 2 ---------------------"<<endl;
    RegisterOGRVT();

    // 
    // Connect DataSource and Layer
    //
    OGRSFDriver   *poDriver = 
        OGRSFDriverRegister::GetRegistrar()->GetDriverByName("Vector tile");

    OGRDataSource *poVTDS = poDriver->Open("VT: kv_store=redis host=192.168.1.166");

    //
    // Prepare data
    //
    OGRLayer *poLayer = poVTDS->GetLayerByName("water");
    OGRLineString geoFilter();
    geoFilter.addPoint();
    geoFilter.addPoint();
    geoFilter.addPoint();
    geoFilter.addPoint();
    geoFilter.addPoint();
    geoFilter.addPoint();
    poLayer->SetSpatialFilter(geoFilter);
    poLayer->SetAttributeFilter();

    //
    // Read Features
    //
    poLayer->ResetReading();
    
    OGRFeatureDefn *poFeatureDefn= poLayer->GetLayerDefn();
    OGRFeature *poFeature = poLayer->GetNextFeature();

    while(poFeature) {
        int fieldCnt = poFeatureDefn->GetFieldCount();
        for(int i = 0; i < fieldCnt; ++i) {
            printField(poFeature, i);
        }
        
        int geomCnt = poFeatureDefn->GetGeomFieldCount();
        for(int i = 0; i < geomCnt; ++i) {
            printGeometry(poFeature, i);
        }

        cout<<endl;
    }
}


