//#include "stdafx.h"
#include "PGAccessor.h"
#include "PGData.h"
#include <ogrsf_frmts.h>
#include <iostream>

const char* const PGAccessor::host= "192.168.1.99";
const char* const PGAccessor::port= "5432";
const char* const PGAccessor::user= "postgres";
const char* const PGAccessor::password= "postgres";
//const char* const PGAccessor::dbname= "china_osm";//中国地区数据库
const char* const PGAccessor::dbname= "dc";//华盛顿地区数据库

using namespace std;

PGAccessor::PGAccessor(OGRSFDriver* poDriver)
{
    ConnDB(poDriver, host,port,user,password,dbname);
}

PGAccessor::~PGAccessor()
{
    if(m_pgDataSource!=NULL)
	{
		OGRSFDriverRegistrar::GetRegistrar()->ReleaseDataSource(m_pgDataSource);
	}
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


OGRLayer* PGAccessor::getLayer(const char *layerName, int layerLevel, double xmin,double ymin,double xmax,double ymax,std::vector<char *> &attrColumn)
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
   const char* pszTableName = GetTableName(layerName, layerLevel);
   sprintf(pszsqlstat,"SELECT %s FROM %s WHERE ST_intersects( ST_MakeEnvelope(%f,%f,%f,%f)::geography::geometry,geom)",//columnStr.c_str(),layerName.c_str(),
		columnStr,
        pszTableName,
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
