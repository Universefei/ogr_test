#include <iostream>

enum EResType
{
    eDBConn = 1,
}

class ResFactory
{

public:
    static ResFactory*      getFactory();
    ResPool*                createPool( EResType );
        
}
