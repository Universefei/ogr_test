#include <iostream>
#include <list>
using namespace std;

/******************************************************************************
 *                                  Resource                                  *
 *****************************************************************************/
class Resource
{
    int             bUsable_;
public:
    Resource();
    virtual ~Resource();

    void            setUsed();
    void            setFree();
    int             bUsable();

    virtual int     isValid() = 0;
    virtual int     validate() = 0;
};

/******************************************************************************
 *                                   ResPool                                  *
 *****************************************************************************/
class ResPool
{
    int                 maxSize_;
    int                 curSize_;
    list<Resource*>     resPool_;
    
public:
    ResPool();
    ResPool(int maxSize);
    virtual ~ResPool();

    virtual int         initPool() = 0;
    virtual void        checkResValid();
    virtual Resource*   getRes();
    virtual void        retRes(Resource*); 
    //virtual int         addRes(int) = 0;
    int                 bHaveUsableRes();
protected:
    virtual int         isResValid(Resource*);
    virtual int         validate(Resource*) ;
    virtual int         deleteRes(Resource*);
};

/******************************************************************************
 *                                   PoolFactory                              *
 *****************************************************************************/
enum EResType
{
    eDBConn = 1,
};

class PoolFactory
{
    static PoolFactory* singleton_;
public:
    PoolFactory();
    ~PoolFactory();
    static PoolFactory* getFactory();
    ResPool*            createPool( EResType );
    ResPool*            createPool( EResType eType, void* initData );
};

