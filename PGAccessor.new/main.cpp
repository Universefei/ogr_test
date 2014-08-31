#include "PGAccessor.h"
#include <utility>
#include <iostream>
//#include <functional>
#include "misc.h"
#include "utilities.h"

using namespace std;

RsltList fei;
RsltList* prsltList = &fei;

void* threadProc(void* arg)
{
    RsltNode *prsltNode = new RsltNode();
    PGAccessor* ppgIns = new PGAccessor(*prsltNode);

    ppgIns->ConnDB("192.168.1.99", "5432", "postgres", "postgres", "china_osm");
    const char* pszSqlState = "SELECT * from planet_osm_polygon where osm_id='26331721'OR osm_id='24426593'";
    ppgIns->SetSQL(const_cast<char*>(pszSqlState));

    ppgIns->DumpRsltToJsonOnMemQue();
    prsltList->push_back(prsltNode);

};



int main()
{
    //RsltList* prsltList = new RsltList();

    pthread_t tid[5];

    /*
    for(int i=0;i<5;++i)
    {
        pthread_create(&tid[i],NULL,threadProc,NULL);
    }
    */
    threadProc(&tid);
    
    cout<<"thread create over"<<endl;
    // loop to wait if result list is empty
    while(prsltList->empty());

    while(!prsltList->empty())
    {
        RsltNode* prsltNode=prsltList->front();
        //prsltList->pop_front();
        
        BufList* pbufList=prsltNode->pcontent;

        JsonSeg* jsonSeg;
        cout<<"start get seg"<<endl;
        while(!GetNextJsonSeg(*prsltNode,&jsonSeg))
        {
            cout<<"show size:       "<<jsonSeg->second<<endl;
            cout<<"show text:       "<<jsonSeg->first<<endl;
        }
        while(prsltList->empty());
    }
    cout<<"print over"<<endl;

    /*
    for(int i=0;i<5;++i)
    {
        pthread_join(tid[i],NULL);
    }
    */
    
    return 0;
}


