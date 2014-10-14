#include "Pool.h"
#include "PGAPool.h"

#define SLEEPTIME 5

/******************************************************************************
 *                         @implementation ResPool                            *
 *****************************************************************************/
ResPool::ResPool()
    :maxSize_(0),curSize_(0)
{
    initPool();
}

ResPool::ResPool(int maxSize)
    :maxSize_( maxSize ),curSize_(0)
{
}

ResPool::~ResPool()
{
    list<Resources*>::iterator pRes;
    for(pRes = resPool_->front(); pRes!=resPool_->end(); ++pRes)
    {
        deleteRes(pRes);
    }
}

Resource* ResPool::getRes()
{
    Resource* pRetRes = NULL;
    list<Resources*>::iterator pRes;
    for(pRes = resPool_->front(); pRes!=resPool_->end(); ++pRes)
    {
        if(pRes->bUsable())
        {
            pRetRes = static_cast<Resource*>(pRes);
            //XXX: does cast successfully?
            pRetRes->setUsed();
            break;
        }
    }
    return pRetRes;
}

void ResPool::checkResValid()
{
    while(true)
    {
        list<Resources*>::iterator pRes;
        for(pRes = resPool_->front(); pRes!=resPool_->end(); ++pRes)
        {
            if(pRes->bUsable())
            {
                if(!isResValid(pRes))
                    validate(pRes);
            }
        }
        sleep(SLEEPTIME);
    }

}

void ResPool::retRes(Resource* pRes)
{
    pRes->setFree();
}

int ResPool::bHaveUsableRes()
{
    int flag = 0;
    list<Resources*>::iterator pRes;
    for(pRes = resPool_->front(); pRes!=resPool_->end(); ++pRes)
    {
        if(pRes->bUsable())
        {
            flag = 1;
            break;
        }
    }
    return flag;
}

int ResPool::isResValid(Resource* pRes)
{
    return pRes->isValid();
}

int ResPool::validate(Resource* pRes)
{
    return pRes->validate();
}

int ResPool::deleteRes(Resource* pRes)
{
    delete pRes;
}
