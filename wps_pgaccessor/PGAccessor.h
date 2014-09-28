#ifndef PGACCESSOR_H
#define PGACCESSOR_H

#pragma warning( disable : 4786 )

#include <vector>
#include <string>


class OGRDataSource;
class OGRLayer;
class OGRSFDriver;

class PGAccessor
{
   
public:
    PGAccessor(OGRSFDriver* poDriver);
    ~PGAccessor();
	//OGRLayer* getLayer(const std::string& layerName,double xmin,double ymin,double xmax,double ymax,std::vector<std::string> *names);
	OGRLayer* getLayer( const char *layerName,double xmin,double ymin,double xmax,double ymax,std::vector<char *> &attrColumn);
private:
	OGRDataSource*	ConnDB(OGRSFDriver* poDriver, const char* host,const char* port,const char* user,const char* password,const char* dbname );
	OGRDataSource*	m_pgDataSource;

	const static char* const host;
	const static char* const port;
	const static char* const user;
	const static char* const password;
	const static char* const dbname;
};

#endif // PGACCESSOR_H

