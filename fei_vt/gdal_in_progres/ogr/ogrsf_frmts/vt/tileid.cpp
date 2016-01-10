#include "tileid.h"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <cstring>
/* #include <stdio.h> */

BBox::BBox(double left_, double up_, double right_, double down_) :
    left(left_), up(up_), right(right_), down(down_)
{
}

BBox::~BBox() 
{}


/* --------------------------------------------------------------------- */
/*                             TileID()                                  */
/* --------------------------------------------------------------------- */

TileID::TileID(const char* pszLayerName, int z, int x, int y):
    x_(x), y_(y), z_(z)
{
    pszLayerName_ = (char*)malloc(strlen(pszLayerName) + 1);
    strcpy(pszLayerName_, pszLayerName);
}


/* TileID::TileID(const char* pszKey) */
/* { */
/*     this->setFromString(pszKey); */
/* } */


TileID::TileID(const TileID& cpy)
{
    x_ = cpy.x_;
    y_ = cpy.y_;
    z_ = cpy.z_;
    pszLayerName_ = (char*)malloc(strlen(cpy.pszLayerName_) + 1);
    strcpy(pszLayerName_, cpy.pszLayerName_);
}

/* --------------------------------------------------------------------- */
/*                             ~TileID()                                 */
/* --------------------------------------------------------------------- */

TileID::~TileID()
{
    if(pszLayerName_)
        free(pszLayerName_);
}

/* --------------------------------------------------------------------- */
/*                             operator =                                */
/* --------------------------------------------------------------------- */

TileID& TileID::operator=(const TileID& duplica)

{
    if(this == &duplica) return *this; /* effective C++ item 11 */

    x_ = duplica.x_;
    y_ = duplica.y_;
    z_ = duplica.z_;
    free(pszLayerName_); pszLayerName_ = NULL;
    pszLayerName_ = (char*)malloc(strlen(duplica.pszLayerName_) + 1);
    strcpy(pszLayerName_, duplica.pszLayerName_);
    return *this;
}

/* --------------------------------------------------------------------- */
/*                             operator ==                               */
/* --------------------------------------------------------------------- */

bool TileID::operator==(const TileID& duplica)

{
    if(this == &duplica) {return true;} 
    return ( x_ == duplica.x_ && y_ == duplica.y_ && z_ == duplica.z_ &&
            !strcmp(duplica.pszLayerName_, pszLayerName_) );
}

/* --------------------------------------------------------------------- */
/*                             setLayerName()                            */
/* --------------------------------------------------------------------- */

int TileID::setLayerName(const char* pszLayerName)
{
    if(pszLayerName_) free(pszLayerName_);
    pszLayerName_ = (char*)malloc(strlen(pszLayerName) + 1);
    strcpy(pszLayerName_, pszLayerName);
    return 0;
}

/* --------------------------------------------------------------------- */
/*                             toString()                                */
/* --------------------------------------------------------------------- */

/**
 *  Notice: User is responsible to free returned memory
 */

const char* TileID::toString() const
{
    char *buf = (char*) malloc(128);
    memset(buf, 0, 128);
    char tmp[128];
    memset(buf, 0, 128);
    memset(tmp, 0, 128);

    strcpy(buf, pszLayerName_);
    sprintf(tmp, "#%d#%d#%d", z_, x_, y_);
    strcat(buf, tmp);

    return buf;
}

/* --------------------------------------------------------------------- */
/*                             setFromString()                           */
/* --------------------------------------------------------------------- */

/* int TileID::setFromString(const char*  pszKey) */
/* { */
/*     char layerName[200]; */
/*     char *buf = (char*) malloc(1024); */
/*     strcpy(buf, pszKey); */
/*     sscanf(buf, "%s#%d#%d#%d", layerName, &z_, &x_, &y_); */
/*     if(pszLayerName_) */
/*     { */
/*         free(pszLayerName_); */
/*         pszLayerName_ = NULL; */
/*     } */
/*     pszLayerName_ = (char*)malloc(strlen(layerName) + 1); */
/*     strcpy(pszLayerName_, layerName); */

/*     free(buf); */
/*     return 0; */
/* } */


/* --------------------------------------------------------------------- */
/*                             getGregraphBBox()                         */
/* --------------------------------------------------------------------- */

#include "math.h"
BBox* TileID::getGeographBBox()
{
    double left = -180.0 + (x_-1)*(360.0/pow(2, z_));
    double up = 90.0 - (y_-1)*(180.0/pow(2, z_-1));
    double right = left + (360.0/pow(2, z_));
    double down = up - (180.0/pow(2, z_-1));
    return new BBox(left, up, right, down);
}



