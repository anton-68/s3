/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
/**
* @file s3_manager.h
* @brief 
* @author Anton Bondarenko
* @version 1
* @date 2012-08-24
*/

#ifndef S3_MANAGER_H
#define S3_MANAGER_H

#include <opal.h>
#include <opal/manager.h>
#include "s3_call.h"
         
class S3Manager : public OpalManager 
{
    PCLASSINFO(S3Manager, OpalManager); 

    public:

        S3Manager();

        virtual S3Call * CreateCall(void *);

};

#endif // S3_MANAGER_H
