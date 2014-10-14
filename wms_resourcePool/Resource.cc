#include "Pool.h"

/******************************************************************************
 *                         @implementation Resource                           *
 *****************************************************************************/
Resource::Resource()
    :bUsable_(0)
{
}

Resource::~Resource()
{
}

void Resource::setUsed()
{
    this->bUsable_ = 0;
}

void Resource::setFree()
{
    this->bUsable_ = 1;
}

int Resource::bUsable()
{
    return this->bUsable_;
}
