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
* @file s3_opal_sdp_cxx_inc.h
* @brief Static non-member code borrowed from OPAL 
* @author Anton Bondarenko
* @version 1
* @date 2012-09-09
*/
#ifndef S3_OPAL_SDP_CXX_INC_H
#define S3_OPAL_SDP_CXX_INC_H
#define SIP_DEFAULT_SESSION_NAME    "Opal SIP Session"
#define SDP_MEDIA_TRANSPORT_RTPAVP  "RTP/AVP"

static const char SDPBandwidthPrefix[] = "SDP-Bandwidth-";

#define new PNEW

//
// uncommment this to output a=fmtp lines before a=rtpmap lines. Useful for testing
//
//#define FMTP_BEFORE_RTPMAP 1

#define SDP_MIN_PTIME 10

/////////////////////////////////////////////////////////

static OpalTransportAddress ParseConnectAddress(const PStringArray & tokens, PINDEX offset, WORD port = 0)
{
  if (tokens.GetSize() == offset+3) {
    if (tokens[offset] *= "IN") {
      if (
        (tokens[offset+1] *= "IP4")
#if OPAL_PTLIB_IPV6
        || (tokens[offset+1] *= "IP6")
#endif
        ) {
        if (tokens[offset+2] == "255.255.255.255") {
          PTRACE(2, "SDP\tInvalid connection address 255.255.255.255 used, treating like HOLD request.");
        }
        else if (tokens[offset+2] == "0.0.0.0") {
          PTRACE(3, "SDP\tConnection address of 0.0.0.0 specified for HOLD request.");
        }
        else {
          OpalTransportAddress address(tokens[offset+2], port, "udp");
          PTRACE(4, "SDP\tParsed connection address " << address);
          return address;
        }
      }
      else
      {
        PTRACE(1, "SDP\tConnect address has invalid address type \"" << tokens[offset+1] << '"');
      }
    }
    else {
      PTRACE(1, "SDP\tConnect address has invalid network \"" << tokens[offset] << '"');
    }
  }
  else {
    PTRACE(1, "SDP\tConnect address has invalid (" << tokens.GetSize() << ") elements");
  }

  return OpalTransportAddress();
}

static OpalTransportAddress ParseConnectAddress(const PString & str, WORD port = 0)
{
  PStringArray tokens = str.Tokenise(' ');
  return ParseConnectAddress(tokens, 0, port);
}

/*
static PString GetConnectAddressString(const OpalTransportAddress & address)
{
  PStringStream str;

  PIPSocket::Address ip;
  if (!address.IsEmpty() && address.GetIpAddress(ip) && ip.IsValid())
    str << "IN IP" << ip.GetVersion() << ' ' << ip.AsString(PTrue);
  else
    str << "IN IP4 0.0.0.0";

  return str;
}
*/
#endif // S3_OPAL_SDP_CXX_INC_H
