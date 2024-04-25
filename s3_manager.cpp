/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
/**
* @file s3_manager.cpp
* @brief 
* @author Anton Bondarenko
* @version 1
* @date 2012-08-24
*/

#include "s3_manager.h"




//         S3Manager   

S3Manager::S3Manager() : OpalManager()
{
    PTRACE(5, "S3\t::S3Manager");
}        




//                  CreateCall

S3Call * S3Manager::CreateCall(void * userData) {
    PTRACE(5, "S3\t->S3Manager::CreateCall");
    return new S3Call(*this);
}
