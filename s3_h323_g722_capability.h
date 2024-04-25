/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
/**
* @file s3_h323_g722_capability.h
* @brief 
* @author Anton Bondarenko
* @version 1
* @date 2012-09-16
*/

#ifndef S3_H323_G711_CAPABILITY_H
#define S3_H323_G711_CAPABILITY_H

#include <h323/h323caps.h>
#include "s3_media_descriptor.h"

class S3H323G722Capability : public H323AudioCapability 
{
    PCLASSINFO(S3H323G722Capability, H323AudioCapability);

    public:

        S3H323G722Capability();

        virtual PObject * Clone() const;

        virtual unsigned GetSubType() const;

        virtual PString GetFormatName()const;
        
};
#endif //  S3_H323_G711_CAPABILITY_H
