#include "Pool.h"
#include "ogrsf_frmts.h"
//#include "PGAccessor.h"

/* class ConnInfo */
/* { */
/*     string          dbname_; */
/*     string          user_; */
/*     string          psw_; */
/*     string          host_; */
/*     string          port_; */
/* public: */
/*     ConnInfo(); */
/*     ConnInfo(string dbname, */
/*              string user, */
/*              string psw, */
/*              string host, */
/*              string port); */
/*     ~ConnInfo(); */

/*     string          getDbname(); */
/*     string          getUser(); */
/*     string          getPsw(); */
/*     string          getHost(); */
/*     string          getPort(); */

/*     void            setDbname(string); */
/*     void            setUser(string); */
/*     void            setPsw(string); */
/*     void            setHost(string); */
/*     void            setPort(string); */
/* } */

/******************************************************************************
 *                                   PGAccessor                               *
 *****************************************************************************/
class PGAccessor : public Resource
{
    //ConnInfo*           pConnInfo_;
    PGAPool*            pPool_;
    OGRDataSource*      m_pgDataSource;
public:
    PGAccessor(PGAPool*);
    virtual ~PGAccessor();

    OGRDataSource*      getDataSource();
    OGRDataSource*      connDB();
	OGRDataSource*	    ConnDB(OGRSFDriver* poDriver, 
                                const char* host,
                                const char* port,
                                const char* user,
                                const char* password,
                                const char* dbname );

	OGRLayer*           getLayer( const char *layerName,
                                double xmin,double ymin,
                                double xmax,double ymax,
                                std::vector<char *> &attrColumn);
protected:
    PGAPool*            getPool();
    void                setPool(PGAPool*);
};

/******************************************************************************
 *                                  PGAPool                                   *
 *****************************************************************************/
class PGAPool : public ResPool
{
    string          dbname_;
    string          user_;
    string          psw_;
    string          host_;
    string          port_;
    /* ConnInfo*       pConnInfo_; */

    OGRSFDriver*    ppgDriver_;

public:
                        PGAPool();
                        PGAPool(int);
                        PGAPool( int maxSize,
                                 string dbname,
                                 string user,
                                 string psw,
                                 string host,
                                 string port,
                                 OGRSFDriver* ppgDriver );
    virtual ~PGAPool();

    //
    // geter and seter
    //
    void                 setConnInfo(string dbname,
                                     string user,
                                     string psw,
                                     string host,
                                     string port);
    void                 setDriver(OGRSFDriver*);
    string               getDbname();
    string               getUser();
    string               getPsw();
    string               getHost();
    string               getPort();

    virtual int          initPool();//
    virtual void         checkResValid();
    virtual Resource*    getRes();
    virtual void         retRes(Resource*); 
    //virtual int          deleteRes(Resource*);//
    //virtual int          addRes(int);//

protected:
    virtual int          isResValid(Resource*);
    virtual int          validate(Resource*) ;
};
