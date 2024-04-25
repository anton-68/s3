/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
/**
* @file s3_descriptor_factory.cpp
* @brief 
* @author Anton Bondarenko
* @version 1
* @date 2012-09-05
*/

#include <ptlib.h>
#include <sip/sdp.h>
#include "s3_descriptor_factory.h"
#include "s3_h264_descriptor.h"




//                                       createDescriptor

S3MediaDescriptor * S3DescriptorFactory::createDescriptor(const H245_DataType & dataType, 
                                     const H245_H2250LogicalChannelParameters & parameters,
                                                                 const unsigned direction, //refactor
                                                                 const unsigned role)      //refactor
{
    PTRACE(5, "S3\t->S3DescriptorFactory::createDescriptor");
    S3MediaDescriptor * descriptor = new S3MediaDescriptor(dataType, parameters, direction, role);
    if (descriptor->isEmpty() || descriptor->isAudio())
        return descriptor;
    
    H245_CapabilityIdentifier capabilityId = ((const H245_GenericCapability &)((const H245_VideoCapability &)dataType)).m_capabilityIdentifier;
    PASN_ObjectId oid = ((PASN_ObjectId)capabilityId);
    PASN_ObjectId itu_h264_oid;
    unsigned num_itu_oid [] = {0,0,8,241,0,0,1};
    PUnsignedArray * pus_itu_oid = new PUnsignedArray(num_itu_oid, 7);
    itu_h264_oid.SetValue(*pus_itu_oid);
    if(capabilityId.GetTag() == H245_CapabilityIdentifier::e_standard  && oid == itu_h264_oid ) 
    {
        delete descriptor;
        S3H264Descriptor * h264Descriptor = new S3H264Descriptor (dataType, parameters, direction, role);
        return h264Descriptor;      
    }
    else 
    {
    /*  descriptor->setEmpty();
        return descriptor;*/
        return NULL;
    }
}




//                                       createDescriptor

S3MediaDescriptor * S3DescriptorFactory::createDescriptor(const OpalMediaType & type,
                                                         const SDPMediaFormat & format,
                                                   const OpalTransportAddress & address,
                                                                   const WORD & port,
                                         const SDPMediaDescription::Direction & direction,
                                                const S3MediaDescriptor::Role & role) 
{
    PTRACE(5, "S3\t->S3DescriptorFactory::createDescriptor");    
    if(type == OpalMediaType::Audio()) 
    {
        PTRACE(5, "S3\tFactory has been requested for audio descriptor for " << format); 
        S3MediaDescriptor * descriptor = new S3MediaDescriptor (type, format, address, port, direction, role);
        return descriptor;
    }
    else 
        if(type == OpalMediaType::Video() /*&& format.GetEncodingName() == "H264"*/) 
        {
            PTRACE(5, "S3\tFactory has been requested for video descriptor for " << format); 
            S3H264Descriptor * h264descriptor = new S3H264Descriptor (type, format, address, port, direction,role);
            return h264descriptor;
        }
        else 
            return NULL;
}   




//                                       createDescriptor

S3MediaDescriptor * S3DescriptorFactory::createDescriptor(const S3MediaDescriptor & descriptor) 
{
    PTRACE(5, "S3\t->S3DescriptorFactory::createDescriptor");
    if(descriptor.isAudio()) 
    {
        S3MediaDescriptor * newDescriptor = new S3MediaDescriptor(descriptor);
        return newDescriptor;
    }
    else
        if(descriptor.isVideo()) 
        {
            S3H264Descriptor * newDescriptor = new S3H264Descriptor(*(static_cast<const S3H264Descriptor*>(&descriptor)));
            return newDescriptor;
        }
    return NULL;
}
