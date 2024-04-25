/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
/**
* @file s3_sip_endpoint.cpp
* @brief 
* @author Anton Bondarenko
* @version 1
* @date 2012-08-24
*/
#include "s3_sip_endpoint.h"
#include "s3_sip_connection.h"




//             S3SIPEndpoint

S3SIPEndpoint::S3SIPEndpoint(S3Manager & manager) : SIPEndPoint(manager) 
{
    PTRACE(5, "S3\t::S3SIPEndpoint");
}




//                               CreateConnection

S3SIPConnection * S3SIPEndpoint::CreateConnection(OpalCall & call,
                                             const PString & token,
                                                    void * /*userData*/,
                                              const SIPURL & destination,
                                             OpalTransport * transport,
                                                 SIP_PDU * /*invite*/,
                                                unsigned int options,
                             OpalConnection::StringOptions * stringOptions)
{
    return new S3SIPConnection(call, *this, token, destination, transport, options, stringOptions);
}
