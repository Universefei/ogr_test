#include "PGAccessor.h"
#include <iostream>
using namespace std;

int main()
{
    BBOX boundingbox;
    boundingbox.xmin = -8584936.075;
    boundingbox.ymin = 4700671.6768;
    boundingbox.xmax = -8564829.929;
    //boundingbox.ymax = 4719819.279;
    boundingbox.ymax = 4710000.279;
    charStream column;
    column.push_back("gid");
    column.push_back("wat_name");
    //column.push_back("");
    column.push_back("geom");

    DataOperation::genFeatureFile("planet_osm_polygon", boundingbox, column);

    cout<<"will return"<<endl;
    return 0;
}

