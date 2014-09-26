#include "wps_ct.h"
#include <ogrsf_frmts.h>
#include <iostream>
using namespace std;

int main (int argc, char const* argv[])

{
    OGRRegisterAll();
    const string srcFile = "input";
    //const string srcSRS = "EPSG:900913";
    const string srcSRS = "EPSG:4326";
    const string dstFile = "output";
    const string dstSRS = "EPSG:900913";
    //const string dstSRS = "EPSG:4326";

    int CTErr = transformate(srcSRS, dstSRS, srcFile, dstFile);
    if(CTErr != 0 )
    {
        cout<<"SRS transformation failed!"<<endl;
    }
    return 0;
}

