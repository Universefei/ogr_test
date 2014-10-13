#include <ogrsf_frmts.h>
#include <iostream>
using namespace std;

int main (int argc, char const* argv[])
{
    OGRSFDriver         *pPGDriver = NULL;
    OGRDataSource       *pPGDS = NULL;
    OGRLayer            *pPGLayer = NULL;
    int                 nLayers = 0;

    OGRRegisterAll();

    pPGDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName("PostgreSQL");
    if(!pPGDriver)
    {
        fprintf(stderr,"GetDriver PostgreSQL error \n");
        exit(1);
    }

    //const char*         pszPGConnInfo = "PG: host='192.168.3.201' port='5432' user='postgres' password='postgres' dbname='us_osm'";
    const char*         pszPGConnInfo = "PG: host='192.168.1.99' port='5432' user='postgres' password='postgres' dbname='dc'";
    try
    {
        pPGDS = pPGDriver->Open(pszPGConnInfo);
    }
    catch(...)
    {
        fprintf(stderr,"Connect PostgreSQL error\n");
    }

    if(!pPGDS)
    {
        fprintf(stderr,"Connect PostgreSQL error\n");
        exit(1);
    }

    nLayers = pPGDS->GetLayerCount();
    int iLyr = 0;
    char* pszSRSWkt = NULL;
    for(; iLyr<nLayers; ++iLyr)
    {
        pPGLayer = pPGDS->GetLayer(iLyr);
        cout<<"Layer "<<iLyr<<": "<<pPGLayer->GetName()<<endl;
        pPGLayer->GetSpatialRef()->exportToWkt(&pszSRSWkt);
        cout<<"        SRS wkt:"<<pszSRSWkt<<endl;
        CPLFree(pszSRSWkt);
    }
    return 0;
}
