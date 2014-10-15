#include <ogrsf_frmts.h>
#include <ogr_core.h> /* OGRwktGeomType */
#include "conn_pg.h"
#include <iostream>
using namespace std;

/******************************************************************************
 *                               main()                                       *
 *****************************************************************************/
                        
OGRDataSource* connPG()
{
    RegisterOGRPG();
    OGRSFDriver* pPGDriver = 
        OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName("PostgreSQL");
    if(!pPGDriver)
    {
        fprintf(stderr,"GetDriver PostgreSQL error \n");
        exit(1);
    }

    const char* pszPGConnInfo = 
        "PG: host='192.168.1.99' port='5432' user='postgres' password='postgres' dbname='dc'";
    //const char* pszPGConnInfo = "PG: host='219.244.118.170' port='5432' user='postgres' password='postgres' dbname='dc'";
    pPGDS = pPGDriver->Open(pszPGConnInfo);
    if(!pPGDS)
    {
        fprintf(stderr,"Connect PostgreSQL error\n");
        exit(1);
    }

    return pPGDS;
}

