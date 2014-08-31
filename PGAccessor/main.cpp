#include "PGAccessor.h"
#include <utility>
#include <iostream>
//#include <functional>

using namespace std;

int main()
{

    BufList JsonQue;

    PGAccessor* ppgIns = new PGAccessor();

    cout<<"connect database"<<endl;
    //ppgIns->ConnDB("192.168.1.99",
    ppgIns->ConnDB("127.0.0.1",
                    "5432",
                    "postgres",
                    "postgres",
                    //"china_osm");
                    "opengeo");

    cout<<"executer sql statement"<<endl;
    //const char* pszSqlState = "SELECT * from planet_osm_polygon where osm_id='26331721'OR osm_id='24426593'";
    //const char* pszSqlState = "SELECT * from planet_osm_polygon where building='yes'";
    const char* pszSqlState = "SELECT * from dc_coastline";
    //const char* pszSqlState = "SELECT * from planet_osm_polygon ";
    ppgIns->ExecuteSQL(pszSqlState);

    cout<<"dump result to json on disk"<<endl;
    const char* pszJsonfile = "jsonDisk.fei";
    //ppgIns->DumpRsltToJsonOnDisk(pszJsonfile);

    cout<<"dump to memQue"<<endl;
    ppgIns->SetQue(&JsonQue);
    ppgIns->DumpRsltToJsonOnMemQue();

    cout<<"list node number:    "<<JsonQue.size()<<endl;

    BufList::iterator   pbufNode;
    for(pbufNode=JsonQue.begin();pbufNode!=JsonQue.end();++pbufNode)
    {
        //cout<<"show test:       "<<(*pbufNode)->first<<endl;
        cout<<"show size:       "<<(*pbufNode)->second<<endl;
        cout<<"show text:       "<<(*pbufNode)->first<<endl;
        //cout<<"show length:     "<<strlen(plistNode->first)<<endl;
    }


    return 0;
}

