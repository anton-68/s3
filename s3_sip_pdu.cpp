/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
/**
* @file s3_sip_pdu.cpp
* @brief 
* @author Anton Bondarenko
* @version 1
* @date 2012-09-10
*/

#include "s3_sip_pdu.h"
#include "s3_opal_sip_pdu_cxx_inc.h"




//         S3SIP_PDU   

S3SIP_PDU::S3SIP_PDU(Methods method)
    : SIP_PDU(method) 
{
    PTRACE(5, "S3\t::S3SIP_PDU");
}




//         S3SIP_PDU

S3SIP_PDU::S3SIP_PDU(const SIP_PDU & request, StatusCodes code, const SDPSessionDescription * sdp)
    : SIP_PDU(request, code, sdp) 
{
    PTRACE(5, "S3\t::S3SIP_PDU");
}




//         S3SIP_PDU

S3SIP_PDU::S3SIP_PDU(const SIP_PDU &pdu)
    : SIP_PDU(pdu) 
{
    PTRACE(5, "S3\t::S3SIP_PDU");    
}




//                                   GetSDP

S3SDPSessionDescription * S3SIP_PDU::GetSDP() 
{
    PTRACE(5, "S3\t->S3SIP_PDU::GetSDP");

    if (m_SDP == NULL && !m_entityBody.IsEmpty() && m_mime.GetContentType() == "application/sdp") 
    {
        m_SDP = new S3SDPSessionDescription(0, 0, OpalTransportAddress());
        if (!static_cast<S3SDPSessionDescription*>(m_SDP)->Decode(m_entityBody)) 
        {
            delete m_SDP;
            m_SDP = NULL;
        }
    }
    return static_cast<S3SDPSessionDescription*>(m_SDP);
}
