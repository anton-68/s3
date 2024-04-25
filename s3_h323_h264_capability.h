/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
/**
* @file s3_h323_h264_capability.h
* @brief 
* @author Anton Bondarenko 
* @version 1
* @date 2012-09-16
*/

#ifndef S3_H323_H264_CAPABILITY_H
#define S3_H323_H264_CAPABILITY_H

#include <h323/h323caps.h>
#include "s3_media_descriptor.h"

class S3H323H264Capability : public H323GenericVideoCapability 
{
    PCLASSINFO(S3H323H264Capability, H323GenericVideoCapability);

    public:

        S3H323H264Capability();
        
        S3H323H264Capability(const S3MediaDescriptor *);

        virtual PObject * Clone() const;

        virtual unsigned GetSubType() const;

        virtual PString GetFormatName() const;

        virtual PBoolean OnSendingPDU(H245_DataType&) const;

    protected:
        
        const S3MediaDescriptor * descriptor;

};
#endif //  S3_H323_H264_CAPABILITY_H
