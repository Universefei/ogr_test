#include "ogrsf_frmts.h"
#include "cpl_string.h"
#include "DataOperation.h"
#include <iostream>
#include <string.h>
#include <sys/stat.h>

//
//  Constructor
DataOperation::DataOperation()
{
}

//
// Destructor
DataOperation::~DataOperation()
{
}

/****************************************************************************/
/*                        genFeatureFile()                                  */
/****************************************************************************/

FileInfo DataOperation::genFeatureFile(const std::string& layerName, 
        const BBOX& boundingBox, const charStream& attrColumn)
{

    
    OGRSFDriver *poDriver;
    srand(time(0));

    // make all column name in one string
    std::string columnStr = "";
    int i=0;
    while (i<attrColumn.size()-1)
    {
        columnStr+=attrColumn[i];
        columnStr+=",";
        i++;
    }
    columnStr+=attrColumn[i];
    
    std::cout<<columnStr<<std::endl;

    /// Register all OGR drivers
    OGRRegisterAll();
    std::cout<<"register driver over"<<std::endl;

    /// Create and Open a PostgreSQL DataSource.
    const char *pszPGDriverName = "PostgreSQL";
    poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszPGDriverName);
    const char *pszConnInfo = "PG: host='192.168.1.99' port='5432' user='postgres' password='postgres' dbname='china_osm'";
    OGRDataSource *poPgDS = poDriver->Open(pszConnInfo);

    std::cout<<"connect db and open driver over"<<std::endl;
    /// Create and Open a GeoJSON DataSource.
    const char *pszGJDriverName = "GeoJson";
    poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszGJDriverName);
    char pszOutFileName[40];
    memset(pszOutFileName,0,40);
    char randchar[10];
    for(int i=0; i<9; i++)
    {
        randchar[i]='a'+(rand()%25);
    }
    randchar[10]='\0';
    strcat(pszOutFileName,"/dev/shm/");
    strcat(pszOutFileName,randchar);
    strcat(pszOutFileName,".GJson");
    //const char *pszOutFileName = "fileName";
    OGRDataSource *poGeojsonDS = poDriver->CreateDataSource(pszOutFileName);

                        //WHERE ST_Contains(ST_MakeEnvelope(%f, %f, %f, %f),geom)\

    /// Get Layer from DataSource query with an SQL clause.
    CPLString* queryClause = new CPLString;
    queryClause->Printf("SELECT * \
                        FROM %s \
                        ",
                         //columnStr.c_str(), 
                         layerName.c_str()//, 
                         //boundingBox.xmin, boundingBox.ymin, 
                         //boundingBox.xmax, boundingBox.ymax
                        );

    std::cout<<queryClause->c_str()<<std::endl;

    // Get a OGRPGResultLayer via executing SQL statement
    OGRLayer  *poPgLayer = poPgDS->ExecuteSQL(queryClause->c_str(), NULL, NULL);
    if(NULL == poPgLayer)
    {
        std::cout<<"executeSQL error"<<std::endl;
    }
    delete(queryClause);

    /// Duplicate retrived Layer to specific output file format.
    OGRLayer    *poGeojsonLayer = poGeojsonDS->CopyLayer(poPgLayer, "query_result");

//add something
//
    OGRSFDriverRegistrar::GetRegistrar()->ReleaseDataSource(poGeojsonDS);

    /// Construct return object.
    FileInfo prtFile; 
    prtFile.fileName=(std::string)pszOutFileName;
    struct stat fStat;
    stat(pszOutFileName,&fStat);
    prtFile.size = (double)fStat.st_size;
    prtFile.pos = 0;
    prtFile.fileId = 0;
    
    std::cout<<"fileName:  "<<prtFile.fileName<<std::endl;
    std::cout<<"fileSize:  "<<prtFile.size<<std::endl;

    return prtFile;
}


/****************************************************************************/
/*                      describeFeatureType()                               */
/****************************************************************************/

FileInfo describeFeatureType(const std::string & layer)
{
    FileInfo rtFile;

}

/****************************************************************************/
/*                          getCapabilities()                               */
/****************************************************************************/
FileInfo getCapabilities()
{
    FileInfo rtFile;
}



