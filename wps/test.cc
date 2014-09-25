#include "wps_ct.h"
#include <iostream>
using namespace std;

int main (int argc, char const* argv[])

{
    const string srcFile = "input";
    const string srcSRS = "EPSG:4326";
    const string dstFile = "output";
    const string dstSRS = "EPSG:900913";

    int CTErr = transformate(srcSRS, dstSRS, srcFile, dstFile);
    if(CTErr != 0 )
    {
        cout<<"SRS transformation failed!"<<endl;
    }
    return 0;
}

