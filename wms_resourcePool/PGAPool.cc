#include "PGAPool.h"


PGAPool::PGAPool()
{
}

PGAPool::~PGAPool()
{
}

PGAPool::PGAPool(int maxSize)
    :ResPool(maxSize)
{
    initPool();
}

PGAPool::PGAPool( int maxSize,
         string dbname,
         string user,
         string psw,
         string host,
         string port,
         OGRSFDriver* ppgDriver )
    :ResPool(maxSize), // call base class's constructor first.
     dbname_(dbname),
     user_(user),
     psw_(psw),
     host_(host),
     port_(port),
     ppgDriver_(ppgDriver)
{
    initPool();
}

void PGAPool::setConnInfo( string dbname,
                           string user,
                           string psw,
                           string host,
                           string port)
{
    this->dbname_ = dbname;
    this->user_ = user;
    this->psw_ = psw;
    this->host_ = host;
    this->port_ = port;
}

int PGAPool::initPool()
{
    for(int idx=0; idx < maxSize_; ++idx)
    {
        PGA* ppga = new PGA();
        ppga->setPool(this);
        ppga->connDB(ppgDriver_, dbname_, user_, psw_, host_, port_ );
        ppga->setFree();
        resPool_->push_back(ppga);
    }
    return 0;
}

