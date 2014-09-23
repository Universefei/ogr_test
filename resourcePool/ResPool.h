#include "ResPool.h"

class ResPool
{
    int                 maxConn_;
    vector<Resource*>   res_;
    
    
    
public:
    ResPool();
    ~ResPool();

    bool                initPool();
    Resource*           getRes();
    
}
