/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
/**
* @file s3_h323_g722_capability.cpp
* @brief 
* @author Anton Bondarenko
* @version 1
* @date 2012-09-16
*/

#include "s3_h323_g722_capability.h"




//                    S3H323G722Capability

S3H323G722Capability::S3H323G722Capability() 
    : H323AudioCapability() 
{
    PTRACE(5, "S3\t::S3H323G722Capability");
}




//                              Clone

PObject * S3H323G722Capability::Clone() const
{
    PTRACE(5, "S3\t->S3H323G722Capability::Clone");
    return new S3H323G722Capability(*this);
}




//                             GetSubType

unsigned S3H323G722Capability::GetSubType() const 
{
    PTRACE(5, "S3\t->S3H323G722Capability::GetSubType");
    return H245_AudioCapability::e_g722_64k;
}




//                            GetFormatName

PString S3H323G722Capability::GetFormatName() const 
{
    PTRACE(5, "S3\t->S3H323G722Capability::GetFormatName");
    return OPAL_G722;
}