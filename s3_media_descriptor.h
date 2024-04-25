/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
/**
* @file s3_media_descriptor.h
* @brief 
* @author Anton Bondarenko
* @version 1
* @date 2012-08-27
*/

#ifndef     S3_MEDIA_DESCRIPTOR_H
#define     S3_MEDIA_DESCRIPTOR_H

#include <ptlib.h>
#include <asn/h245.h>
#include <sip/sdp.h>
#include "s3_sdp.h"
#include "s3_h323_connection.h"

class S3MediaDescriptor : public PObject 
{
    PCLASSINFO(S3MediaDescriptor, PObject);

    friend ostream &operator<< (ostream &, const S3MediaDescriptor &);

    friend PBoolean operator==(const S3MediaDescriptor & lEx, const S3MediaDescriptor &);

    friend PBoolean operator!=(const S3MediaDescriptor & lEx, const S3MediaDescriptor &);

    public:

        enum Protocol
        {
            h323,
            sip,
            undefined
        };

        enum Role 
        {
            originator,
            terminator
        };

        enum Type 
        {
            audio,
            video,
            unsupported
        };

        enum H323AudioTag 
        {
            g711ALaw64k = 1,
            g711ULaw64k = 3,
            g722_64k = 5
        };

        enum H323VideoTag 
        {
            genericVideoCapability = 5,
            h264 = 1024
        };

        static RTP_DataFrame::PayloadTypes payloadType(const unsigned); 

        virtual unsigned getPayloadType() const {return payloadNumber;} 

        static PString encodingName(const unsigned);

        virtual PString getPayloadName() const {return typeName;}

        virtual unsigned s3Type(unsigned);

        S3MediaDescriptor() {capabilityTag = -1;}

        S3MediaDescriptor(const S3MediaDescriptor &);

        S3MediaDescriptor(const H245_DataType &,
                          const H245_H2250LogicalChannelParameters &,
                          const unsigned,
                          const unsigned);

        S3MediaDescriptor(const OpalMediaType &,
                          const SDPMediaFormat &,
                          const OpalTransportAddress &,
                          const WORD &,
                          const SDPMediaDescription::Direction &,
                          const S3MediaDescriptor::Role &);

        virtual PBoolean merge(S3MediaDescriptor &);        

        virtual PBoolean mergeH323OLC(S3MediaDescriptor &);     

        virtual PBoolean isEmpty() const {return capabilityTag==static_cast<unsigned>(-1);}

        WORD P_DEPRECATED getOriginatorPort() const {return mediaPort[originator];} // deprecated

        WORD P_DEPRECATED getTerminatorPort() const {return mediaPort[terminator];} // deprecated

        virtual WORD getMediaPort(unsigned role) const {return mediaPort[role];}

        /*virtual*/ WORD getMediaPortFromIP(unsigned) const;

        OpalTransportAddress P_DEPRECATED getOriginatorIP() const {return mediaIP[originator];} // deprecated

        OpalTransportAddress P_DEPRECATED getTerminatorIP() const {return mediaIP[terminator];} // deprecated

        virtual OpalTransportAddress getMediaIP(unsigned role) const {return mediaIP[role];} 

        virtual Protocol getProtocol(const Role & role) {return protocol[role];}

        void setEmpty() {capabilityTag=-1;}

        virtual PBoolean isComplete();      

        virtual PBoolean isVideo() const {return mediaType==video;}

        virtual PBoolean isAudio() const {return mediaType==audio;}

        virtual PBoolean isH323toSIP() const {return protocol[originator] == h323 &&
                                    protocol[terminator] == sip;}

        virtual PBoolean isSIPtoH323() const {return protocol[originator] == sip &&
                                    protocol[terminator] == h323;}      

        virtual S3MediaDescriptor & operator=(const S3MediaDescriptor &);

        virtual ostream& s3print(ostream&) const;

        virtual PBoolean s3compare(const S3MediaDescriptor &) const;

        virtual S3SDPMediaFormat * getSDPFormat(SDPMediaDescription &) const;

        virtual H323Capability * getOpalCapability() const;

        virtual unsigned getOLCNumber(S3H323Connection::ChannelDirection dir) const {return olcNumber[dir];}        

        virtual void setOLCNumber(S3H323Connection::ChannelDirection dir, unsigned channel) {olcNumber[dir] = channel;}     

    protected:

        Type mediaType;

        unsigned payloadNumber;

        PString typeName;       

        unsigned capabilityTag;

        Protocol protocol[2];

        OpalTransportAddress mediaIP[2];

        WORD mediaPort[2];
        
        unsigned olcNumber[2];
};

typedef PList<S3MediaDescriptor> S3MediaDescriptorList;

typedef PList<S3MediaDescriptor>::const_iterator S3MediaDescriptorListIterator;

#endif  //  S3_MEDIA_DESCRIPTOR_H
