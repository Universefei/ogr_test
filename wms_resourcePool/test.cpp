#include "PGAPool.h"

int main (int argc, char const* argv[])
{
    PGAPool* ppgaPool = PoolFactory::getFactory().createPool(EResType);
    ppgaPool->initPool();

    list<task*> queryList;

    /**
     * this is main thread, get a task from queryList then create a thread and 
     * allocate a PGA resouce to handle it.
     */

    while(1)
    {
        if(!queryList->empty())
        {
            task* ptask = queryList.pop_front();
            if(ppgaPool->bHaveUsableRes())
            {
                PGA* ppga = ppgaPool->getRes();
            }
            pthread_create();
        }
    }

    return 0;
}
