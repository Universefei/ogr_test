#include "PGAccessor.h"
#include <utility>
#include <iostream>
//#include <functional>

using namespace std;

int main()
{

/* -------------------------------------------------------------------------- */
    /// Step1:  instanate object.
/* -------------------------------------------------------------------------- */
    PGAccessor* ppgIns = new PGAccessor();

/* -------------------------------------------------------------------------- */
    /// Step2:  connect database.
/* -------------------------------------------------------------------------- */
    cout<<"connect database"<<endl;
    ppgIns->ConnDB("192.168.1.99",
    //ppgIns->ConnDB("127.0.0.1",
                    "5432",
                    "postgres",
                    "postgres",
                    "china_osm");
                    //"opengeo");

/* -------------------------------------------------------------------------- */
    /// Step3:  assemble sql statement.
/* -------------------------------------------------------------------------- */
    //
    // Test set
    //
    //const char* pszSqlState = "SELECT * from planet_osm_polygon ";
    //const char* pszSqlState = "SELECT * from planet_osm_polygon where building='yes'";
    //const char* pszSqlState = "SELECT * from planet_osm_polygon where area='yes'";
    const char* pszSqlState = "SELECT * from planet_osm_polygon where osm_id='26331721'OR osm_id='24426593'";
    //const char* pszSqlState = "SELECT * from dc_coastline";

    ppgIns->SetSQL(const_cast<char*>(pszSqlState));

/* -------------------------------------------------------------------------- */
    /// Step4:  get result.
/* -------------------------------------------------------------------------- */
    //
    // Dump to Disk
    //
    /*
    cout<<"dump result to json on disk"<<endl;
    const char* pszJsonfile = "jsonDisk.fei";
    ppgIns->DumpRsltToJsonOnDisk(pszJsonfile);
    cout<<"write over"<<endl;
    */

    //
    // Dump to memory list
    //
    cout<<"dump to memQue"<<endl;
    ppgIns->DumpRsltToJsonOnMemQue();
    cout<<"dump over"<<endl;

    BufList* pList = ppgIns->GetRsltList();
    int listSize = pList->size();

    pair<char*, int>* plistNode;
    while(!ppgIns->GetNextJsonSeg(&plistNode))
    {
        cout<<"show size:       "<<plistNode->second<<endl;
        cout<<"show text:       "<<plistNode->first<<endl;

    }
    cout<<"list node number:    "<<listSize<<endl;

    delete ppgIns;
    return 0;
}

