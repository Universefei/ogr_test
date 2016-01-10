#include "tileid.h"
#include <iostream>
using namespace std;

void test_tileid() {
    TileID* poTID = new TileID("feilunzhou", 8, 12, 12);

    cout<< poTID->toString()<<endl;

    BBox* poB = poTID->getGeographBBox();
    cout<<"left is : "<<poB->left<<endl;
    cout<<"up is : "<<poB->up<<endl;
    cout<<"right is : "<<poB->right<<endl;
    cout<<"down is : "<<poB->down<<endl;

}
