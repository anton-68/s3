/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
/**
* @file s3_sip_pdu.h
* @brief 
* @author Anton Bondarenko 
* @version 1
* @date 2012-09-10
*/

#include <ptlib.h>
#include <sip/sippdu.h>
#include "s3_sdp.h"

class S3SIP_PDU : public SIP_PDU 
{
    PCLASSINFO(S3SIP_PDU, SIP_PDU);

    public:

        S3SIP_PDU(Methods method = SIP_PDU::NumMethods);

        S3SIP_PDU(const SIP_PDU & request, StatusCodes code, const SDPSessionDescription * sdp = NULL);

        S3SIP_PDU(const SIP_PDU &);

        S3SDPSessionDescription * GetSDP();
};
