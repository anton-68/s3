/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
/**
* @file s3.h
* @brief 
* @author Anton Bondarenko
* @version 1
* @date 2012-08-24
*/

#ifndef S3_H
#define S3_H

#include <ptlib.h>
#include <opal.h>
#include "s3_manager.h"
#include "s3_h323_endpoint.h"
#include "s3_sip_endpoint.h"

#define PRODUCT_NAME_TEXT   "S3"
//#define   EXE_NAME_TEXT   "s3"
#define MANUFACTURER_TEXT   "S3"
#define COPYRIGHT_HOLDER    "Anton Bondarenko"

#define MAJOR_VERSION 0
#define MINOR_VERSION 1
#define BUILD_TYPE ReleaseCode
#define BUILD_NUMBER 1
         
class S3 : public PProcess 
{
    PCLASSINFO(S3,  PProcess);

    public:

        S3();

        void Main();

    protected:

        S3Manager * manager;
        
};

#endif // S3_H
