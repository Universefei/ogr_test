#include <iostream>
#include "utilities.h"

/****************************************************************************/
/*                                IsQueEmpty()                              */
/****************************************************************************/

bool IsQueEmpty(RsltNode rsltNode)
{
    pthread_mutex_t*    pmutex_ = &(rsltNode.mutex_);
    pthread_mutex_lock(pmutex_);
    bool isEmpty = rsltNode.pcontent->empty();
    pthread_mutex_unlock(pmutex_);
    return isEmpty;
}

/****************************************************************************/
/*                               GetNextJsonSeg()                           */
/****************************************************************************/

/**
 * @brief:  pop a node from result list.
 *
 * @return: 1 if reach the end and returned pair is unuseable, else return 0.
 * @ppair:  a BufList node get from the front.
 */
int GetNextJsonSeg(RsltNode rsltNode,JsonSeg** ppair)
{
    cout<<"start get"<<endl;
    pthread_mutex_t*    pmutex_ = &(rsltNode.mutex_);

    cout<<"1 get"<<endl;
    pthread_mutex_lock(pmutex_);
    cout<<"2 get"<<endl;
    *ppair = rsltNode.pcontent->front();
    cout<<"3 get"<<endl;
    rsltNode.pcontent->pop_front();
    cout<<"4 get"<<endl;
    pthread_mutex_unlock(pmutex_);
    
    cout<<"end get"<<endl;

    // check if reach the end.
    if( NULL==(*ppair)->first && (*ppair)->second==0 )
    {
        return 1;
    }
    return 0;
}

