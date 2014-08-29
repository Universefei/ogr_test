#include "PGAccessor.h"
#include <iostream>

using namespace std;

int main()
{

    BufList     JsonQue;
    cout<<"new pgaccessor"<<endl;
    PGAccessor* ppgIns = new PGAccessor();

    cout<<"connect database"<<endl;
    ppgIns->ConnDB("192.168.1.99",
                    "5432",
                    "postgres",
                    "postgres",
                    "china_osm");

    cout<<"executer sql statement"<<endl;
    const char* pszSqlState = "SELECT * from planet_osm_polygon where area='yes'";
    ppgIns->ExecuteSQL(pszSqlState);

    cout<<"dump result to json on disk"<<endl;
    const char* pszJsonfile = "jsonDisk.fei";
    ppgIns->DumpRsltToJsonOnDisk(pszJsonfile);

    //
    //ppgIns->SetQue(&JsonQue);
    //ppgIns->DumpRsltToJsonOnMemQue();

    return 0;
}

