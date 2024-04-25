/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
/**
* @file s3_call.h
* @brief 
* @author Anton Bondarenko
* @version 1
* @date 2012-08-28
*/

#ifndef S3_CALL_H
#define S3_CALL_H

#include <opal/call.h>
#include "s3_media_descriptor.h"
#include "s3_descriptor_factory.h"

//typedef PList<S3MediaDescriptor> S3MediaDescriptorList;
//typedef PList<S3MediaDescriptor>::const_iterator S3MediaDescriptorListIterator;

class S3Call : public OpalCall 
{
    PCLASSINFO(S3Call, OpalCall);

    public:

        S3Call(OpalManager &);

        PBoolean P_DEPRECATED addOriginatorCapability(S3MediaDescriptor &); //deprecated

        virtual PBoolean addDescriptor(S3MediaDescriptor &, const S3MediaDescriptor::Role);
        
        virtual PBoolean checkSelect();

        virtual S3DescriptorFactory* getDescriptorFactory() {return &descriptorFactory;}

        virtual const S3MediaDescriptorList * getMediaDescriptorList(S3MediaDescriptor::Role role) const {return &(capabilities[role]);}

        virtual PBoolean descriptorListIsEmpty(const S3MediaDescriptor::Role role) {return capabilities[role].IsEmpty();} 

        virtual S3MediaDescriptor * getSelectedDescriptor(S3MediaDescriptor::Type media);

        virtual PBoolean OnConnected(OpalConnection &);

    protected:

        S3DescriptorFactory descriptorFactory;

        S3MediaDescriptorList capabilities[2];

        S3MediaDescriptorList sharedCapabilities;

        S3MediaDescriptor * selectedDescriptor[2];      
};
#endif // S3_CALL_H
