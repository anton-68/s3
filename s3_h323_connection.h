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
* @file s3_h323_connection.h
* @brief Overwritten functionality of OPAL H323Connection. See details in .cpp-file.
* @author Anton Bondarenko
* @version 1
* @date 2012-08-25
*/

#ifndef S3_H323_CONNECTION_H
#define S3_H323_CONNECTION_H

#include <ptlib.h>
#include <opal.h>
#include <h323/h323.h>
#include <h323/h323con.h>
#include <h323/h323pdu.h>
#include "s3_sdp.h"

class S3MediaDescriptor;

class S3H323Connection : public H323Connection 
{
    PCLASSINFO(S3H323Connection, H323Connection);

    public:

		enum ChannelDirection {
			forward,
			reverse
		};

        S3H323Connection(OpalCall & call,
            			 H323EndPoint & endpoint,  
						 const PString & token,   
                         const PString & alias,  
                         const H323TransportAddress & address,
                         unsigned options = 0,               
                         OpalConnection::StringOptions * stringOptions = NULL);

		virtual PBoolean OnReceivedSignalSetup(const H323SignalPDU &);

		virtual PBoolean processFastStartOffer(const H225_ArrayOf_PASN_OctetString & offer);

		virtual PBoolean SetConnected();

		virtual PBoolean SendFastStartAcknowledge(H225_ArrayOf_PASN_OctetString & array);
		
		virtual PBoolean OnCreateLogicalChannel(const H323Capability & capability,  ///<  Capability for the channel open
      											H323Channel::Directions dir,  ///<  Direction of channel
      											unsigned & errorCode);        ///<  Error to return if refused


		virtual H323Channel * CreateLogicalChannel(const H245_OpenLogicalChannel & open, ///<  Parameters for opening channel
      											   PBoolean startingFast,                 ///<  Flag for fast/slow starting.
      											   unsigned & errorCode,                  ///<  Reason for create failure
												   const S3MediaDescriptor * descriptor); // S3 descriptor

		virtual H323Channel * CreateRealTimeLogicalChannel(const H323Capability & capability, ///<  Capability creating channel
      													   H323Channel::Directions dir,   ///<  Direction of channel
      													   unsigned sessionID,            ///<  Session ID for RTP channel
      													   const H245_H2250LogicalChannelParameters * param,
                                         												  ///<  Parameters for channel
      													   RTP_QOS * rtpqos = NULL);      ///<  QoS for RTP 

	    virtual PBoolean IsMediaBypassPossible(unsigned sessionID) const;
};
#endif //  S3_H323_CONNECTION_H
