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
* @file s3_opal_sip_pdu_cxx_inc.h
* @brief Static non-member code borrowed from OPAL 
* @author Anton Bondarenko 
* @version 1
* @date 2012-09-10
*/
#ifndef S3_OPAL_SIP_PDU_CXX_INC_H
#define S3_OPAL_SIP_PDU_CXX_INC_H
#define SIP_VER_MAJOR  2
#define SIP_VER_MINOR  0
#define new PNEW

// static bool LocateFieldParameter(const PString & fieldValue, const PString & paramName, PINDEX & start, PINDEX & val, PINDEX & end);

////////////////////////////////////////////////////////////////////////////

static const char * const MethodNames[SIP_PDU::NumMethods] = {
  "INVITE",
  "ACK",
  "OPTIONS",
  "BYE",
  "CANCEL",
  "REGISTER",
  "SUBSCRIBE",
  "NOTIFY",
  "REFER",
  "MESSAGE",
  "INFO",
  "PING",
  "PUBLISH",
  "PRACK"
};

#if PTRACING
ostream & operator<<(ostream & strm, SIP_PDU::Methods method)
{
  if (method < SIP_PDU::NumMethods)
    strm << MethodNames[method];
  else
    strm << "SIP_PDU_Method<" << (unsigned)method << '>';
  return strm;
}
#endif


const char * SIP_PDU::GetStatusCodeDescription(int code)
{
  static struct {
    int code;
    const char * desc;
  } sipErrorDescriptions[] = {
    { SIP_PDU::Information_Trying,                  "Trying" },
    { SIP_PDU::Information_Ringing,                 "Ringing" },
    { SIP_PDU::Information_CallForwarded,           "Call Forwarded" },
    { SIP_PDU::Information_Queued,                  "Queued" },
    { SIP_PDU::Information_Session_Progress,        "Progress" },

    { SIP_PDU::Successful_OK,                       "OK" },
    { SIP_PDU::Successful_Accepted,                 "Accepted" },

    { SIP_PDU::Redirection_MultipleChoices,         "Multiple Choices" },
    { SIP_PDU::Redirection_MovedPermanently,        "Moved Permanently" },
    { SIP_PDU::Redirection_MovedTemporarily,        "Moved Temporarily" },
    { SIP_PDU::Redirection_UseProxy,                "Use Proxy" },
    { SIP_PDU::Redirection_AlternativeService,      "Alternative Service" },

    { SIP_PDU::Failure_BadRequest,                  "BadRequest" },
    { SIP_PDU::Failure_UnAuthorised,                "Unauthorised" },
    { SIP_PDU::Failure_PaymentRequired,             "Payment Required" },
    { SIP_PDU::Failure_Forbidden,                   "Forbidden" },
    { SIP_PDU::Failure_NotFound,                    "Not Found" },
    { SIP_PDU::Failure_MethodNotAllowed,            "Method Not Allowed" },
    { SIP_PDU::Failure_NotAcceptable,               "Not Acceptable" },
    { SIP_PDU::Failure_ProxyAuthenticationRequired, "Proxy Authentication Required" },
    { SIP_PDU::Failure_RequestTimeout,              "Request Timeout" },
    { SIP_PDU::Failure_Conflict,                    "Conflict" },
    { SIP_PDU::Failure_Gone,                        "Gone" },
    { SIP_PDU::Failure_LengthRequired,              "Length Required" },
    { SIP_PDU::Failure_RequestEntityTooLarge,       "Request Entity Too Large" },
    { SIP_PDU::Failure_RequestURITooLong,           "Request URI Too Long" },
    { SIP_PDU::Failure_UnsupportedMediaType,        "Unsupported Media Type" },
    { SIP_PDU::Failure_UnsupportedURIScheme,        "Unsupported URI Scheme" },
    { SIP_PDU::Failure_BadExtension,                "Bad Extension" },
    { SIP_PDU::Failure_ExtensionRequired,           "Extension Required" },
    { SIP_PDU::Failure_IntervalTooBrief,            "Interval Too Brief" },
    { SIP_PDU::Failure_TemporarilyUnavailable,      "Temporarily Unavailable" },
    { SIP_PDU::Failure_TransactionDoesNotExist,     "Call Leg/Transaction Does Not Exist" },
    { SIP_PDU::Failure_LoopDetected,                "Loop Detected" },
    { SIP_PDU::Failure_TooManyHops,                 "Too Many Hops" },
    { SIP_PDU::Failure_AddressIncomplete,           "Address Incomplete" },
    { SIP_PDU::Failure_Ambiguous,                   "Ambiguous" },
    { SIP_PDU::Failure_BusyHere,                    "Busy Here" },
    { SIP_PDU::Failure_RequestTerminated,           "Request Terminated" },
    { SIP_PDU::Failure_NotAcceptableHere,           "Not Acceptable Here" },
    { SIP_PDU::Failure_BadEvent,                    "Bad Event" },
    { SIP_PDU::Failure_RequestPending,              "Request Pending" },
    { SIP_PDU::Failure_Undecipherable,              "Undecipherable" },

    { SIP_PDU::Failure_InternalServerError,         "Internal Server Error" },
    { SIP_PDU::Failure_NotImplemented,              "Not Implemented" },
    { SIP_PDU::Failure_BadGateway,                  "Bad Gateway" },
    { SIP_PDU::Failure_ServiceUnavailable,          "Service Unavailable" },
    { SIP_PDU::Failure_ServerTimeout,               "Server Time-out" },
    { SIP_PDU::Failure_SIPVersionNotSupported,      "SIP Version Not Supported" },
    { SIP_PDU::Failure_MessageTooLarge,             "Message Too Large" },

    { SIP_PDU::GlobalFailure_BusyEverywhere,        "Busy Everywhere" },
    { SIP_PDU::GlobalFailure_Decline,               "Decline" },
    { SIP_PDU::GlobalFailure_DoesNotExistAnywhere,  "Does Not Exist Anywhere" },
    { SIP_PDU::GlobalFailure_NotAcceptable,         "Not Acceptable" },

    { SIP_PDU::Local_TransportError,                "Transport Error" },
    { SIP_PDU::Local_BadTransportAddress,           "Invalid Address/Hostname" },
    { SIP_PDU::Local_Timeout,                       "Timeout or retries exceeded" },

    { 0 }
  };

  for (PINDEX i = 0; sipErrorDescriptions[i].code != 0; i++) {
    if (sipErrorDescriptions[i].code == code)
      return sipErrorDescriptions[i].desc;
  }
  return "";
}


ostream & operator<<(ostream & strm, SIP_PDU::StatusCodes status)
{
  strm << (unsigned)status;
  const char * info = SIP_PDU::GetStatusCodeDescription(status);
  if (info != NULL && *info != '\0')
    strm << ' ' << info;
  return strm;
}


static struct {
  char         compact;
  const char * full;
} const CompactForms[] = {
  { 'l', "Content-Length" },
  { 'c', "Content-Type" },
  { 'e', "Content-Encoding" },
  { 'f', "From" },
  { 'i', "Call-ID" },
  { 'm', "Contact" },
  { 's', "Subject" },
  { 't', "To" },
  { 'v', "Via" },
  { 'r', "Refer-To" },
  { 'b', "Referred-By" },
  { 'k', "Supported" },
  { 'o', "Event" }
};
#endif // S3_OPAL_SIP_PDU_CXX_INC_H

