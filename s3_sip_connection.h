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
* @file s3_sip_connection.h
* @brief 
* @author Anton Bondarenko
* @version 1
* @date 2012-09-07
*/

#ifndef S3_SIP_CONNECTION_H
#define S3_SIP_CONNECTION_H

#include <ptlib.h>
#include <sip/sip.h>
#include <h323/q931.h>
#include "s3_sip_pdu.h"

class S3SIPConnection : public SIPConnection 
{
    PCLASSINFO(S3SIPConnection, SIPConnection);

    public:

    enum ChannelDirection 
    {
        forward,
        reverse
    };

    S3SIPConnection(OpalCall &,
                    SIPEndPoint &,  
                    const PString &,     
                    const SIPURL &,
                    OpalTransport *,
                    unsigned options = 0,               
                    OpalConnection::StringOptions * stringOptions = NULL);

    PBoolean SetUpConnection();

    virtual void OnCreatingINVITE(SIPInvite &);

    virtual bool OnSendOfferSDPSession(const OpalMediaType &,
                                       unsigned,
                                       OpalRTPSessionManager &,
                                       SDPSessionDescription &,
                                       bool);

    virtual void OnReceivedResponseToINVITE(SIPTransaction &, SIP_PDU &);

    virtual void OnReceivedAnswerSDP(SIP_PDU &);

    virtual PBoolean IsMediaBypassPossible(unsigned) const;

    virtual PBoolean processSDPAnswer(const S3SDPSessionDescription &);
};
#endif //  S3_SIP_CONNECTION_H
