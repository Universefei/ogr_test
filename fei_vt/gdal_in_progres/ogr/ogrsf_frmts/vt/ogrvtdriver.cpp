/******************************************************************************
 * $Id: ogrmemdriver.cpp 10645 2007-01-18 02:22:39Z warmerdam $
 *
 * Project:  OpenGIS Simple Features Reference Implementation
 * Purpose:  Implements OGRVTDriver class.
 * Author:   Frank Warmerdam, warmerdam@pobox.com
 *
 ******************************************************************************
 * Copyright (c) 2003, Frank Warmerdam <warmerdam@pobox.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ****************************************************************************/

#include "ogr_vt.h"
#include "cpl_conv.h"
#include "cpl_string.h"

CPL_CVSID("$Id: ogrvtdriver.cpp 10645 2007-01-18 02:22:39Z warmerdam $");

/************************************************************************/
/*                          ~OGRVTDriver()                           */
/************************************************************************/

OGRVTDriver::~OGRVTDriver()

{
}

/************************************************************************/
/*                              GetName()                               */
/************************************************************************/

const char *OGRVTDriver::GetName()

{
    return "Vector Tile";
}

/************************************************************************/
/*                                Open()                                */
/************************************************************************/

/**
 * feilunzhou
 * @pszDSConnInfo: all info contained to connect kv datasource
 */

OGRDataSource *OGRVTDriver::Open( const char * pszDSConnInfo, int )

{
    OGRVTDataSource     *poDS;

    poDS = new OGRVTDataSource();

    if( !poDS->Open( pszDSConnInfo, 0, TRUE ) )
    {
        delete poDS;
        return NULL;
    }
    else
        return poDS;
}

/************************************************************************/
/*                          CreateDataSource()                          */
/************************************************************************/

OGRDataSource *OGRVTDriver::CreateDataSource( const char * pszName,
                                               char **papszOptions )

{
    return new OGRVTDataSource( pszName, papszOptions );
}

/************************************************************************/
/*                           TestCapability()                           */
/************************************************************************/

int OGRVTDriver::TestCapability( const char * pszCap )

{
    if( EQUAL(pszCap,ODrCCreateDataSource) )
        return TRUE;
    else
        return FALSE;
}

/************************************************************************/
/*                           RegisterOGRVT()                           */
/************************************************************************/
 #include "test_includes.h"

void RegisterOGRVT()

{
    OGRSFDriverRegistrar::GetRegistrar()->RegisterDriver( new OGRVTDriver );
    printf("feilunzhou, test vt");
    printf("--------------------------\n");
    test_tileid();
}

