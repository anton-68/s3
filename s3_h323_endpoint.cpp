/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
/**
* @file s3_h323_endpoint.cpp
* @brief 
* @author Anton Bondarenko
* @version 1
* @date 2012-08-24
*/

#include "s3_h323_endpoint.h"
#include "s3_h323_connection.h"




//              S3H323Endpoint

S3H323Endpoint::S3H323Endpoint(S3Manager & manager) : H323EndPoint(manager)
{
    PTRACE(5, "S3\t::S3H323Endpoint");
}




//                               CreateConnection

H323Connection * S3H323Endpoint::CreateConnection(OpalCall & call,
                                             const PString & token,
                                                      void * userData, 
                                             OpalTransport & transport, 
                                             const PString & alias, 
                                const H323TransportAddress & address, 
                                             H323SignalPDU * setupPDU,
                                                    unsigned options,
                             OpalConnection::StringOptions * stringOptions) 
{
    PTRACE(5, "S3\t->S3H323Endpoint::CreateConnection");
    return new S3H323Connection(call, *this, token, alias, address, options, stringOptions);
}
