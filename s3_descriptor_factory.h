/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
/**
* @file s3_descriptor_factory.h
* @brief 
* @author Anton Bondarenko
* @version 1
* @date 2012-09-05
*/

#ifndef S3_DESCRIPTOR_FACTORY_H
#define S3_DESCRIPTOR_FACTORY_H

#include "s3_media_descriptor.h"

class S3DescriptorFactory : public PObject 
{
    PCLASSINFO(S3DescriptorFactory, PObject);

    public:

        S3DescriptorFactory() 
        {
        }

        S3MediaDescriptor * createDescriptor(const H245_DataType &, 
                                             const H245_H2250LogicalChannelParameters &,
                                             const unsigned,
                                             const unsigned);

        S3MediaDescriptor * createDescriptor(const OpalMediaType &,
                                             const SDPMediaFormat &,
                                             const OpalTransportAddress &,
                                             const WORD &,
                                             const SDPMediaDescription::Direction &,
                                             const S3MediaDescriptor::Role &);

        S3MediaDescriptor * createDescriptor(const S3MediaDescriptor &);

};
#endif // S3_DESCRIPTOR_FACTORY_H
