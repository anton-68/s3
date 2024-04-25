/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
/**
* @file s3_h323_h264_capability.cpp
* @brief 
* @author Anton Bondarenko
* @version 1
* @date 2012-09-16
*/

#include <codec/opalplugin.h>
#include "s3_h323_h264_capability.h"
#include "s3_h264_descriptor.h"




//                    S3H323H264Capability

S3H323H264Capability::S3H323H264Capability() 
    : H323GenericVideoCapability(OpalPluginCodec_Identifer_H264_Generic /*"0.0.8.241.0.0.1"*/ ) 
{
    PTRACE(5, "S3\t::S3H323H264Capability");
}



//                    S3H323H264Capability

S3H323H264Capability::S3H323H264Capability(const S3MediaDescriptor * descriptor) 
    : H323GenericVideoCapability(OpalPluginCodec_Identifer_H264_Generic /*"0.0.8.241.0.0.1"*/ ) 
{
    this->descriptor = descriptor;
    PTRACE(5, "S3\t::S3H323H264Capability");
}



//                              Clone

PObject * S3H323H264Capability::Clone() const
{
    PTRACE(5, "S3\t->S3H323H264Capability::Clone");
    return new S3H323H264Capability(*this);
}



//                             GetSubType

unsigned S3H323H264Capability::GetSubType() const 
{
    PTRACE(5, "S3\t->S3H323H264Capability::GetSubType");
    return H245_VideoCapability::e_genericVideoCapability;
}



//                            GetFormatName

PString S3H323H264Capability::GetFormatName() const {
    PTRACE(5, "S3\t->S3H323H264Capability::GetFormatName");
    return OPAL_H264;
}



//                             OnSendingPDU

PBoolean S3H323H264Capability::OnSendingPDU(H245_DataType& dt) const {
    PTRACE(5, "S3\t->S3H323H264Capability::OnSendingPDU");

    H323VideoCapability::OnSendingPDU(dt);

    H245_GenericCapability * gc = &((H245_GenericCapability &)((H245_VideoCapability &)dt));
    gc->IncludeOptionalField(H245_GenericCapability::e_collapsing);
    H245_ArrayOf_GenericParameter * collapsing = &(gc->m_collapsing);

    // Profile

    H245_ParameterIdentifier * profileId = new H245_ParameterIdentifier(H245_ParameterIdentifier::e_standard);
    (PASN_Integer &)(*profileId) = 41;

    H245_ParameterValue * profileValue = new H245_ParameterValue(H245_ParameterValue::e_booleanArray);
    (PASN_Integer &)(*profileValue) = static_cast<const S3H264Descriptor*>(descriptor)->getH264Profile();

    H245_GenericParameter * profile = new H245_GenericParameter();
    profile->m_parameterIdentifier = *profileId;
    profile->m_parameterValue = *profileValue;
    collapsing->Append(profile);

    // Level
    
    H245_ParameterIdentifier * levelId = new H245_ParameterIdentifier();
    levelId->SetTag(H245_ParameterIdentifier::e_standard);
    (PASN_Integer &)(*levelId) = 42;

    H245_ParameterValue * levelValue = new H245_ParameterValue();
    levelValue->SetTag(H245_ParameterValue::e_unsignedMin);
    (PASN_Integer &)(*levelValue) = static_cast<const S3H264Descriptor*>(descriptor)->getH264Level();

    H245_GenericParameter * level = new H245_GenericParameter();
    level->m_parameterIdentifier = *levelId;
    level->m_parameterValue = *levelValue;
    collapsing->Append(level);
    
    return true;
}
