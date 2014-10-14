#include "Pool.h"

/******************************************************************************
 *                         @implementation PoolFactory                        *
 *****************************************************************************/
PoolFactory::PoolFactory()
{
    if(singleton_ == NULL)
    {
        singleton_ = new PoolFactory();
    }
}

PoolFactory::~PoolFactory()
{
}

PoolFactory* PoolFactory::getFactory()
{
    return this.singleton_;
}

ResPool* PoolFactory::createPool(EResType eType)
{
    switch(eType)
    {
        case eDBConn:
        {
            return new PGA();
            break;
        }
        case eMem:
        case eThread:
        default:
        {
            break;
        }
    }
}


