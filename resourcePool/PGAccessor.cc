#include "PGAPool.h"
#include <iostream>

using namespace std;
//#include "libpq-fe.h"


int PGAccessor::isValid()
{
    //OGRPGDatasource ;
    //FIXME:how to check if a pgaccessor is valid?
    PGConn* pConn = this->getDataSource()->GetPGConn();
    return PQStatus(pConn)==CONNECTION_OK;
}

int PGAccessor::validate()
{
    //FIXME:how to validate pgaccessor.
    this->connDB();
}

/******************************************************************************
 *                                                                            *
 * 
 *                                                                            *
 *****************************************************************************/

PGAccessor::PGAccessor(PGAPool* pPool)
    :pPool_(pPool)
{
    connDB();
}

PGAccessor::~PGAccessor()
{
    if(m_pgDataSource!=NULL)
	{
		OGRSFDriverRegistrar::GetRegistrar()->ReleaseDataSource(m_pgDataSource);
	}
}

OGRDataSource* PGAccessor::getPool()
{
    return pPool_;
}

void PGAccessor::setPool(PGAPool* pPool)
{
    pPool_ = pPool;
}

OGRDataSource* PGAccessor::getDataSource()
{
    return m_pgDataSource;
}

OGRDataSource* PGAccessor::ConnDB(OGRSFDriver* poDriver,
								  const char* host,
								  const char* port,
								  const char* user,
								  const char* password,
								  const char* dbname )
{
    char* pszConnInfo = (char*)malloc(sizeof(char)*1000);
    sprintf(pszConnInfo,"PG: host='%s' port='%s' user='%s' password='%s' dbname='%s'",
		host,port,user,password,dbname);

    OGRDataSource *poPgDS = poDriver->Open(pszConnInfo);

	
	this->m_pgDataSource = poPgDS;
	free(pszConnInfo);
    return poPgDS;
}

OGRDataSource* PGAccessor::connDB()
{
    if( NULL != pPool_)
    {
        return connDB(pPool_->getDriver(),
                        pPool_->getHost().c_str(),
                        pPool_->getPort().c_str()
                        pPool_->getUser().c_str(),
                        pPool_->getPsw().c_str(),
                        pPool_->getDbname().c_str(),
                        );
    }
    else
        return NULL;
}


OGRLayer* PGAccessor::getLayer(const char *layerName,
                                double xmin,double ymin,
                                double xmax,double ymax,
                                std::vector<char *> &attrColumn)
{

	// make all column name in one string
	char* columnStr = (char *)malloc(1024);
	memset(columnStr,0,sizeof(columnStr));
	int i;
	for(i=0;i<attrColumn.size();i++)
	{
		strcat(columnStr, attrColumn.at(i));
		if(i!=attrColumn.size()-1)
		{	
			strcat(columnStr, ",");
		}
	}

   char* pszsqlstat=(char*)malloc(1024*10);
   sprintf(pszsqlstat,"SELECT %s FROM %s WHERE ST_intersects( ST_MakeEnvelope(%f,%f,%f,%f)::geography::geometry,geom)",//columnStr.c_str(),layerName.c_str(),
		columnStr,
		layerName,
		xmin, ymin, 
		xmax, ymax
	);  
   
    OGRLayer* pRetLayer = NULL;


	pRetLayer = m_pgDataSource->ExecuteSQL(pszsqlstat, NULL, NULL);
	if (!pRetLayer)
	{
		cout<<"you get a null layer!"<<endl;
	}
    return pRetLayer;
}
