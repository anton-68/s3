/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
/**
* @file s3_sip_endpoint.h
* @brief 
* @author Anton Bondarenko
* @version 1
* @date 2012-08-24
*/

#ifndef S3_SIP_ENDPOINT_H
#define S3_SIP_ENDPOINT_H

#include <sip/sipep.h>
#include "s3_manager.h"
#include "s3_sip_connection.h"

class S3SIPEndpoint : public SIPEndPoint
{
    PCLASSINFO(S3SIPEndpoint, SIPEndPoint);

    public:

        S3SIPEndpoint(S3Manager &);

        virtual S3SIPConnection * CreateConnection(OpalCall &,
                                                   const PString &,
                                                   void *,
                                                   const SIPURL &, 
                                                   OpalTransport *, 
                                                   SIP_PDU *, 
                                                   unsigned int options = 0,  
                                                   OpalConnection::StringOptions * stringOptions = NULL);
};
#endif //  S3_SIP_ENDPOINT_H
