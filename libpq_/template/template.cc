#include "libpq-fe.h"
#include <iostream>
#include <stdlib.h>
using namespace std;

int main (int argc, char const* argv[])
{
    PGconn          *pConn = NULL;
    PGresult        *pResult = NULL;
    
    /* --------------------------------------------------------------------- */
    /*     Step1: Connect PostgreSQL                                         */
    /* --------------------------------------------------------------------- */
    const char*     pszConnInfo = "host='192.168.1.99' port='5432' \
                                   user='postgres' password='postgres' dbname='china_osm'";
    pConn = PQconnectdb(pszConnInfo);

    if(pConn == NULL || PQstatus(pConn) == CONNECTION_BAD)
    {
        fprintf(stderr, "connected db with connect info:%s failed \n",pszConnInfo);
        exit(1);
    }

    /* --------------------------------------------------------------------- */
    /*     Step2: Execute SQL and handle Result                              */
    /*     ====================================                              */
    /*     [1]:PQexec()                                                      */
    /*     [2]:PQresultStatus(), PQntuples()                                 */
    /*     [3]:PQgetvalue()                                                  */
    /*     [4]:PQclear()                                                     */
    /* --------------------------------------------------------------------- */
    
    const char*     pszSQL = "select f_table_catalog, f_table_name, \
        f_geometry_column, srid, type \
        from geometry_columns";
    pResult = PQexec(pConn, pszSQL);

    if( !pResult || PQresultStatus(pResult) != PGRES_COMMAND_OK )
    {
        /* Do come logic here */
    }

    if( pResult && PQresultStatus(pResult) == PGRES_TUPLES_OK 
            && PQntuples(pResult) > 0 )
    {
        int nRecord = PQntuples(pResult);
        int iRecord;
        cout<<"record acount: "<<nRecord<<endl;
        for( iRecord = 0; iRecord < nRecord; ++iRecord )
        {
            const char* pszDBName = PQgetvalue( pResult, iRecord, 0 );
            const char* pszTableName = PQgetvalue( pResult, iRecord, 1 );
            const char* pszGeomColumnName = PQgetvalue( pResult, iRecord, 2 );
            const char* pszSrid = PQgetvalue( pResult, iRecord, 3 );
            const char* pszGeomType = PQgetvalue( pResult, iRecord, 4 );

            int         srid = atoi(pszSrid);

            cout<<"DB Name:     "<<pszDBName<<endl;
            cout<<"Table Name:  "<<pszTableName<<endl;
            cout<<"Geom column: "<<pszGeomColumnName<<endl;
            cout<<"SRID:        "<<srid<<endl;
            cout<<"Geom Type:   "<<pszGeomType<<endl;
            cout<<endl;
            /* do some logic here */
        }
    }

    PQclear(pResult);
    pResult == NULL;

    /* --------------------------------------------------------------------- */
    /*     Step3: Close PostgreSQL connection                                */
    /* --------------------------------------------------------------------- */
    PQfinish(pConn);

    return 0;
}
