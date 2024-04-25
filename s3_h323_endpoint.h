/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
/**
* @file s3_h323_endpoint.h
* @brief 
* @author Anton Bondarenko
* @version 1
* @date 2012-08-24
*/

#ifndef S3_H323_ENDPOINT_H
#define S3_H323_ENDPOINT_H

#include <h323/h323.h>
#include "s3_manager.h"
#include "s3_h323_connection.h"

class S3H323Endpoint : public H323EndPoint 
{
    PCLASSINFO(S3H323Endpoint, H323EndPoint);

    public:

        S3H323Endpoint(S3Manager &);

        virtual H323Connection * CreateConnection(OpalCall &,
                                                  const PString &,
                                                  void *, 
                                                  OpalTransport &,
                                                  const PString &, 
                                                  const H323TransportAddress &, 
                                                  H323SignalPDU *, 
                                                  unsigned options = 0,
                                                  OpalConnection::StringOptions * stringOptions = NULL);
};
#endif //  S3_H323_ENDPOINT_H
