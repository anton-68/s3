/* 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. 
 *
 * This Source Code Form is "Incompatible With Secondary Licenses", as
 * defined by the Mozilla Public License, v. 2.0.
 * 
 * The Original Code is Open Phone Abstraction Library available at
 * http://www.opalvoip.org.
 *
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * Contributor(s): rjongbloed. 
 */
/**
* @file s3_sdp.h
* @brief 
* @author Anton Bondarenko 
* @version 1
* @date 2012-09-09
*/

#ifndef S3_SDP_H
#define S3_SDP_H
#include <ptlib.h>
#include <sip/sdp.h>

class S3SDPMediaFormat : public SDPMediaFormat
{
    PCLASSINFO(S3SDPMediaFormat, SDPMediaFormat);

    public: 

        S3SDPMediaFormat(SDPMediaDescription &,
                         RTP_DataFrame::PayloadTypes,
                         unsigned payloadNumber,
                         const char * name = NULL);

        bool PreEncode(){return true;}

        void PrintOn(ostream &) const;

        virtual unsigned getPayloadNumber() const {return payloadNumber;}

        bool s3PostDecode(const OpalMediaFormatList &);

        PString getFMTPString() const {return m_fmtp;}

    protected:

        unsigned payloadNumber;
};

class S3SDPAudioMediaDescription : public SDPAudioMediaDescription
{
    PCLASSINFO(S3SDPAudioMediaDescription, SDPAudioMediaDescription);

    public:

        S3SDPAudioMediaDescription(const OpalTransportAddress &);

        virtual PBoolean SetTransportAddress(const OpalTransportAddress &);

        virtual bool PreEncode() {return true;}

        virtual PString GetSDPPortList() const;

        virtual S3SDPMediaFormat * CreateSDPMediaFormat(const PString &);

        virtual bool PostDecode(const OpalMediaFormatList &);
};

class S3SDPVideoMediaDescription : public SDPVideoMediaDescription
{
    PCLASSINFO(S3SDPVideoMediaDescription, SDPVideoMediaDescription);

    public:

        S3SDPVideoMediaDescription(const OpalTransportAddress &);

        virtual PBoolean SetTransportAddress(const OpalTransportAddress &);

        virtual bool PreEncode() {return true;}

        virtual PString GetSDPPortList() const;

        virtual S3SDPMediaFormat * CreateSDPMediaFormat(const PString &);

        virtual bool PostDecode(const OpalMediaFormatList &);
};

class S3SDPSessionDescription : public SDPSessionDescription
{
    PCLASSINFO(S3SDPSessionDescription, SDPSessionDescription);

    public:

        S3SDPSessionDescription(time_t, unsigned, const OpalTransportAddress &);

        virtual bool Decode(const PString &);

        virtual SDPMediaDescription::Direction GetDirection() const {return SDPMediaDescription::SendRecv;} 

        void s3Print(ostream &);
};

#endif //  S3_SDP_H
