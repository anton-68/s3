/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
/**
* @file s3_media_descriptor.cpp
* @brief 
* @author Anton Bondarenko
* @version 1
* @date 2012-08-27
*/

#include <opal/mediafmt.h>
#include "s3_media_descriptor.h"
#include "s3_h264_descriptor.h"
#include "s3_h323_g722_capability.h"

class S3H264Descriptor;




//                 S3MediaDescriptor

S3MediaDescriptor::S3MediaDescriptor(const H245_DataType & dataType,
                const H245_H2250LogicalChannelParameters & parameters,
                                            const unsigned direction,
                                            const unsigned role) 
{
    PTRACE(5, "S3\t::S3MediaDescriptor");
    switch (dataType.GetTag()) 
    {   
        case H245_DataType::e_videoData :
            mediaType = video;
            break;
        case H245_DataType::e_audioData :
            mediaType = audio;
            break;
        default :
            mediaType = unsupported; 
    }

    mediaPort[originator] = 0;
    mediaPort[terminator] = 0;
    
    mediaIP[originator] = OpalTransportAddress("udp$127.0.0.1");
    mediaIP[terminator] = OpalTransportAddress("tcp$127.0.0.1");

    olcNumber[S3H323Connection::reverse] = 0;
    olcNumber[S3H323Connection::forward] = 0;
    
    const H245_UnicastAddress_iPAddress * address = NULL;
    if (mediaType == video)
    {
        capabilityTag = dataType.GetObject().GetTag();
        if (capabilityTag != genericVideoCapability)   // to refactor
        {
            PTRACE(2, "S3\tUnsupported capability: " <<
                    ((const H245_VideoCapability &)dataType).GetTagName() <<
                    ", capability tag: " << capabilityTag);
            capabilityTag = -1; // unsupported video-codec
        }
        else 
        { 
            if(parameters.m_mediaChannel.GetObject().GetTag() == H245_TransportAddress::e_unicastAddress) 
            {
                if(direction == S3H323Connection::reverse)  //Rx
                {
                    address = &((const H245_UnicastAddress_iPAddress &)((const H245_UnicastAddress &)parameters.m_mediaChannel));
                    mediaPort[role] = address->m_tsapIdentifier.GetValue();
                    mediaIP[role] = OpalTransportAddress(PIPSocket::Address(4, address->m_network), mediaPort[role]/*, "upd"*/);
                    PTRACE(5, "S3\tPreparing addr: " << mediaIP[role] << " from port " << mediaPort[role]
                                                    << " and H245 address : " << address);
                }
            }
            else 
                capabilityTag = -1; // non-unicast media ip
        }
    }
    else
    { 
        if (mediaType == audio) { // (mediaType == audio)
            capabilityTag = dataType.GetObject().GetTag();
            if (capabilityTag != g711ALaw64k && 
                capabilityTag != g711ULaw64k &&
                capabilityTag != g722_64k ) { // to remove this code
                PTRACE(2, "S3\tUnsupported capability of " << mediaType << " media type" <<
                        ", capability tag: " << capabilityTag <<
                        ", tag name: " <<  ((const H245_AudioCapability &)dataType).GetTagName());
                capabilityTag = -1; // unsupported audio codec
            }
            else 
            {
                if(parameters.m_mediaChannel.GetObject().GetTag() == H245_TransportAddress::e_unicastAddress) 
                {
                    if(direction == S3H323Connection::reverse)  //Rx
                    {
                        address = &((const H245_UnicastAddress_iPAddress &)((const H245_UnicastAddress &)parameters.m_mediaChannel));
                        //mediaIP[role] = address->m_network;       
                        //mediaPort[role] = address->m_tsapIdentifier;
                        mediaPort[role] = address->m_tsapIdentifier.GetValue();
                        mediaIP[role] = OpalTransportAddress(PIPSocket::Address(4, address->m_network), mediaPort[role]/*, "upd"*/);        
                    }
                }
                else 
                    capabilityTag = -1; // non-unicast media ip
            }
        }
        else capabilityTag = -1; // unsupported media type
    }
    if(!this->isEmpty()) 
    {
        typeName = encodingName(capabilityTag);
    /*  typeName = mediaType==audio? 
            ((const H245_AudioCapability &)dataType).GetTagName()
        :   ((const H245_VideoCapability &)dataType).GetTagName(); */
        protocol[role] = h323;
        payloadNumber = payloadType(capabilityTag);
    }
}




//                 S3MediaDescriptor

S3MediaDescriptor::S3MediaDescriptor(const OpalMediaType & type,
                                     const SDPMediaFormat & format,
                                     const OpalTransportAddress & address,
                                     const WORD & port,
                                     const SDPMediaDescription::Direction & direction,
                                     const S3MediaDescriptor::Role & role) 
{
    PTRACE(5, "S3\t::S3MediaDescriptor");

    if(type == OpalMediaType::Audio())
        mediaType = audio;
    else 
        if(type == OpalMediaType::Video())
            mediaType = video;
        else    
            mediaType = unsupported;
    
    mediaPort[originator] = 0;
    mediaPort[terminator] = 0;

    mediaIP[originator] = OpalTransportAddress("udp$127.0.0.1");
    mediaIP[terminator] = OpalTransportAddress("tcp$127.0.0.1");
    
    olcNumber[S3H323Connection::reverse] = 0;
    olcNumber[S3H323Connection::forward] = 0;
                
    payloadNumber = static_cast<const S3SDPMediaFormat*>(&format)->getPayloadNumber();
    capabilityTag = s3Type(payloadNumber);
    typeName = encodingName(capabilityTag);
    protocol[role] = sip;
    mediaIP[role] = address;
    mediaPort[role] = port;
}




//                                             =

S3MediaDescriptor & S3MediaDescriptor::operator=(const S3MediaDescriptor & rhs)
{
    this->mediaType = rhs.mediaType;
    this->typeName = rhs.typeName;
    this->payloadNumber = rhs.payloadNumber;
    this->capabilityTag = rhs.capabilityTag;
    this->protocol[originator] = rhs.protocol[originator];
    this->protocol[terminator] = rhs.protocol[terminator];
    this->mediaIP[originator] = rhs.mediaIP[originator];
    this->mediaIP[terminator] = rhs.mediaIP[terminator];
    this->mediaPort[originator] = rhs.mediaPort[originator];
    this->mediaPort[terminator] = rhs.mediaPort[terminator];
    this->olcNumber[S3H323Connection::reverse] = rhs.olcNumber[S3H323Connection::reverse];
    this->olcNumber[S3H323Connection::forward] = rhs.olcNumber[S3H323Connection::forward];
    return *this;
}




//                 S3MediaDescriptor

S3MediaDescriptor::S3MediaDescriptor(const S3MediaDescriptor & rhe) 
{
    PTRACE(5, "S3\t::S3MediaDescriptor");
    *this = rhe;
}




//                          merge

PBoolean S3MediaDescriptor::merge(S3MediaDescriptor & descriptor) 
{
    PTRACE(5, "S3\t->S3MediaDescriptor::merge");

    if(mergeH323OLC(descriptor)) 
    {
        if(this->isVideo() && this->getPayloadName() == "H264")
            return static_cast<S3H264Descriptor* const>(this)->merge(*(static_cast<S3H264Descriptor*>(&descriptor)));
        else
            return true;
    }
    PTRACE(2, "S3\t[]merge() error in mergeH323OLC() call");
    return false;
}




//                          mergeH323OLC

PBoolean S3MediaDescriptor::mergeH323OLC(S3MediaDescriptor & descriptor) 
{
    PTRACE(5, "S3\t->S3MediaDescriptor::mergeH323OLC");

    if(descriptor != *this) 
    {
        PTRACE(2, "S3\t[]mergeH323OLC error: not compatible descriptors");
        return false;
    }

    S3MediaDescriptor::Role role;
    for(int i = 0; i < 2; i ++) 
    {
        role = static_cast<Role>(i);
        if(descriptor.getMediaPort(role) != 0) 
        { 
            mediaPort[role] = descriptor.getMediaPort(role);
            mediaIP[role] = OpalTransportAddress(descriptor.getMediaIP(role));
            protocol[role] = descriptor.getProtocol(role);
        }
    }

    if(olcNumber[S3H323Connection::reverse] == 0)
        olcNumber[S3H323Connection::reverse] = descriptor.getOLCNumber(S3H323Connection::reverse);

    if(olcNumber[S3H323Connection::forward] == 0)
        olcNumber[S3H323Connection::forward] = descriptor.getOLCNumber(S3H323Connection::forward);

    return true;
}




//                                             payloadType

RTP_DataFrame::PayloadTypes S3MediaDescriptor::payloadType(unsigned capabilityTag)
{
    PTRACE(5, "S3\t->S3MediaDescriptor::payloadType");
    switch (capabilityTag) 
    {
        case g711ALaw64k :
            return RTP_DataFrame::PCMA;
            break;
        case g711ULaw64k :
            return RTP_DataFrame::PCMU;
            break;
        case g722_64k :
            return RTP_DataFrame::G722;
            break;
        default :
            return RTP_DataFrame::IllegalPayloadType;
    }
}




//                          s3Type

unsigned S3MediaDescriptor::s3Type(unsigned payloadNumber)
{
    PTRACE(5, "S3\t->S3MediaDescriptor::s3Type");
    switch (payloadNumber) 
    {
        case RTP_DataFrame::PCMA :
            return g711ALaw64k;
            break;
        case RTP_DataFrame::PCMU :
            return g711ULaw64k;
            break;
        case RTP_DataFrame::G722 :
            return g722_64k;
            break;
        default :
            return -1;
    }
}




//                         encodingName

PString S3MediaDescriptor::encodingName(unsigned capabilityTag)
{
    PTRACE(5, "S3\t->S3MediaDescriptor::encodingName");
    switch (capabilityTag) 
    {
        case g711ALaw64k :
            return "PCMA";
            break;
        case g711ULaw64k :
            return "PCMU";
            break;
        case g722_64k :
            return "G722";
            break;
        default :
            return "IllegalPayloadType";
    }
}




//                                    getSDPFormat

S3SDPMediaFormat * S3MediaDescriptor::getSDPFormat(SDPMediaDescription & sdpDescriptor) const
{
    PTRACE(5, "S3\t->S3MediaDescriptor::getSDPFormat");

    S3SDPMediaFormat * sdpFormat = NULL;
    //RTP_DataFrame::PayloadTypes pType = payloadType(capabilityTag);
    //const char * eName = static_cast<const char *>(encodingName(capabilityTag));
    sdpFormat = new S3SDPMediaFormat(sdpDescriptor, 
                                   static_cast<RTP_DataFrame::PayloadTypes>(payloadType(capabilityTag)), 
                                   payloadNumber,
                                   encodingName(capabilityTag)); 
    sdpFormat->SetClockRate(8000);  
    return sdpFormat;
}




//                                  getOpalCapability

H323Capability * S3MediaDescriptor::getOpalCapability() const 
{
    PTRACE(5, "S3\tSMediaDescriptor::getOpalCapability");

    H323Capability * capability = NULL;
    OpalMediaFormat format;
    if(isAudio()) 
    {
        switch (capabilityTag) 
        {
            case g711ALaw64k :
                capability = new H323_G711Capability(H323_G711Capability::ALaw);
                format = GetOpalG711_ALAW_64K();
                break;
            case g711ULaw64k :
                capability = new H323_G711Capability();
                format = GetOpalG711_ULAW_64K();
                break;
            case g722_64k :
                capability = new S3H323G722Capability();
                format = GetOpalG722();
                break;
            default :
                return NULL;
                break;
        }
        capability->UpdateMediaFormat(format);
        return capability;
    }
    PTRACE(2, "S3\tMediaDescriptor::getOpalCapability failed (wrong type)");
    return NULL;
}




//               ==

PBoolean operator==(const S3MediaDescriptor & lEx, const S3MediaDescriptor & rEx)
{ 
    return lEx.s3compare(rEx);
} 
        



//               !=

PBoolean operator!=(const S3MediaDescriptor & lEx, const S3MediaDescriptor & rEx) 
{
    return !lEx.s3compare(rEx);
}




//                          s3Compare

PBoolean S3MediaDescriptor::s3compare(const S3MediaDescriptor & rEx) const 
{
    PTRACE(5, "S3\tSMediaDescriptor::s3compare");
    return this->mediaType == rEx.mediaType && 
           this->capabilityTag == rEx.capabilityTag &&
           this->payloadNumber == rEx.payloadNumber;
}




//               <<

ostream &operator<< (ostream & os, const S3MediaDescriptor & descriptor) 
{
    return descriptor.s3print(os);
}




//                           s3print

ostream & S3MediaDescriptor::s3print(ostream & os) const 
{
    os << "S3 Media Descriptor for " << (isVideo()?"video":"audio") << " codec " << typeName 
       << " (payload number = " <<  payloadNumber << ")"  << " : " << endl;
    int role = originator;
    os << (protocol[role]==h323?"h323":"sip") << ':' << mediaIP[role] << ", port = " << mediaPort[role] << " --> ";
    role = terminator;
    os << (protocol[role]==h323?"h323":"sip") << ':' << mediaIP[role] << ", port = " << mediaPort[role] << endl;
    os << "Forward OLC number = " << olcNumber[S3H323Connection::forward] << ", reverse OLC number = " << olcNumber[S3H323Connection::reverse] <<endl;
    return os;
}




//                          isComplete

PBoolean S3MediaDescriptor::isComplete() 
{
    PTRACE(5, "S3\tSMediaDescriptor::isComplete");

    if(typeName == "")
        typeName = encodingName(capabilityTag);

    if((mediaType != video && mediaType != audio) ||
       (payloadNumber >= RTP_DataFrame::LastKnownPayloadType && payloadNumber < RTP_DataFrame::DynamicBase) ||
       (payloadNumber > RTP_DataFrame::MaxPayloadType) ||
       (payloadNumber == RTP_DataFrame::IllegalPayloadType) ||
       (capabilityTag == 0) || 
       (capabilityTag > h264) ||
       (protocol[originator] != h323) || 
       (protocol[terminator] != sip) ||
       mediaIP[originator].IsEmpty() ||
       mediaIP[terminator].IsEmpty() ||
       (mediaPort[originator] == 0) ||
       (mediaPort[terminator] == 0) ) 
    {
        PTRACE(1, "S3\tS3MediaDescription incomplete :");
        s3print(cout);
        return false;
    }
    return true;
}




//                      getMediaPortFromIP

WORD S3MediaDescriptor::getMediaPortFromIP(unsigned role) const
{
    PTRACE(5, "S3\t->S3MediaDescriptor::getMediaPort");
    
    WORD port;
    PIPSocket::Address addr;
    if(mediaIP[role].GetIpAndPort(addr, port))
        return port;
    PTRACE(2, "S3\tgetMediaPort error");
    return -1;
}
