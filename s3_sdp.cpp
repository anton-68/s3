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
* @file s3_sdp.cpp
* @brief Partially borrowed and changed OPAL code for SDP handling
* @author Anton Bondarenko 
* @version 1
* @date 2012-09-09
*/

#include "s3_sdp.h"
#include "s3_opal_sdp_cxx_inc.h"
#include "s3_opal_rtp_cxx_inc.h"




//          S3SDPMediaFormat

S3SDPMediaFormat::S3SDPMediaFormat(SDPMediaDescription & parent, 
                             RTP_DataFrame::PayloadTypes pt, 
                                                unsigned pNumber,
                                            const char * _name)
            : SDPMediaFormat(parent, pt, _name),
              payloadNumber(pNumber)
{
    PTRACE(5, "S3\tS::3SDPMEdiaFormat");
}




//                  GetSDPPortList

PString S3SDPVideoMediaDescription::GetSDPPortList() const 
{
    PTRACE(5, "S3\t->S3SDPVideoMediaDescription::GetSDPPortList");

    if (formats.IsEmpty())
        return " 127"; // Have to have SOMETHING

    PStringStream str;
    const S3SDPMediaFormat * s3format = NULL;
    SDPMediaFormatList::const_iterator format;

    // output RTP payload types. 
    // S3: Cast added:
    for (format = formats.begin(); format != formats.end(); ++format) 
    {
        s3format = static_cast<const S3SDPMediaFormat*>(&(*format));
        str << ' ' << s3format->getPayloadNumber();
    }
    return str;
}




//                  GetSDPPortList

PString S3SDPAudioMediaDescription::GetSDPPortList() const 
{
    PTRACE(5, "S3\t->S3SDPAudioMediaDescription::GetSDPPortList");

    if (formats.IsEmpty())
        return " 127"; // Have to have SOMETHING

    PStringStream str;
    const S3SDPMediaFormat * s3format = NULL;
    SDPMediaFormatList::const_iterator format;

    // output RTP payload types
    // S3: Cast added:
    for (format = formats.begin(); format != formats.end(); ++format) 
    {
        s3format = static_cast<const S3SDPMediaFormat*>(&(*format));
        str << ' ' << s3format->getPayloadNumber();
    }
    return str;
}




//                     PrintOn

void S3SDPMediaFormat::PrintOn(ostream & strm) const {
  
    PAssert(!encodingName.IsEmpty(), "SDPMediaFormat encoding name is empty");

    PINDEX i;
    for (i = 0; i < 2; ++i) 
    {
        switch (i) 
        {
#ifdef FMTP_BEFORE_RTPMAP
            case 1:
#else
            case 0:
#endif
                strm << "a=rtpmap:" << payloadNumber << ' ' << encodingName << '/' << clockRate;
                if (!parameters.IsEmpty())
                    strm << '/' << parameters;
                strm << "\r\n";
                break;
#ifdef FMTP_BEFORE_RTPMAP
            case 0:
#else
            case 1:
#endif
            {
                PString fmtpString = GetFMTP();
                if (!fmtpString.IsEmpty())
                    strm << "a=fmtp:" << payloadNumber << ' ' << fmtpString << "\r\n";
            }
        }
    }
}




//                          S3SDPAudioMediaDescription

S3SDPAudioMediaDescription::S3SDPAudioMediaDescription(const OpalTransportAddress & address)
                        : SDPAudioMediaDescription(address)
{
    PTRACE(5, "S3\t::S3SDPAudioMediaDescription");
}




//                                   SetTransportAddress

PBoolean S3SDPAudioMediaDescription::SetTransportAddress(const OpalTransportAddress &t) 
{
    PTRACE(5, "S3\t->->S3SDPAudioMediaDescription::SetTransportAddress");

    PIPSocket::Address ip;
    WORD p = 0;
    if (t.GetIpAndPort(ip, p)) 
    {
        transportAddress = t;
        port = p;
        return PTrue;
    }
    return PFalse;
}




//                          S3SDPVideoMediaDescription

S3SDPVideoMediaDescription::S3SDPVideoMediaDescription(const OpalTransportAddress & address)
                        : SDPVideoMediaDescription(address)
{
    PTRACE(5, "S3\t::S3SDPVideoMediaDescription");
}




//                                   SetTransportAddress

PBoolean S3SDPVideoMediaDescription::SetTransportAddress(const OpalTransportAddress &t) {
    PTRACE(5, "S3\t->S3SDPVideoMediaDescription::SetTransportAddress");

    PIPSocket::Address ip;
    WORD p = 0;
    if (t.GetIpAndPort(ip, p)) 
    {
        transportAddress = t;
        port = p;
        return PTrue;
    }
    return PFalse;
}




//             S3SDPSessionDescription

S3SDPSessionDescription::S3SDPSessionDescription(time_t sessionId, 
                         unsigned version,
                             const OpalTransportAddress & address)
    : SDPSessionDescription(sessionId, version, address)
{
    PTRACE(5, "S3\t::S3SDPSessionDescription");
}




//                s3print

void S3SDPSessionDescription::s3Print(ostream & os) 
{
    SDPSessionDescription::PrintOn(os);
    PString connectString;
    for (PINDEX i = 0; i < mediaDescriptions.GetSize(); i++) 
    {
        connectString = "IN IP4 ";
        connectString += mediaDescriptions[i].GetTransportAddress().GetHostName();
        mediaDescriptions[i].PrintOn(os, connectString);
    }
}




//                Decode 

bool S3SDPSessionDescription::Decode(const PString & str) {

  PTRACE(5, "S3\t->->S3SDPSessionDescription::Decode");

  OpalMediaFormatList mediaFormats;

  bool atLeastOneValidMedia = false;
  bool ok = true;

  // break string into lines
  PStringArray lines = str.Lines();

  // parse keyvalue pairs
  SDPMediaDescription * currentMedia = NULL;
  PINDEX i;
  for (i = 0; i < lines.GetSize(); i++) {
    const PString & line = lines[i];
    if (line.GetLength() < 3 || line[1] != '=')
      continue; // Ignore illegal lines

    PString value = line.Mid(2).Trim();

    /////////////////////////////////
    //
    // Session description
    //
    /////////////////////////////////
  
    if (currentMedia != NULL && line[0] != 'm')
      currentMedia->Decode(line[0], value);
    else {
      switch (line[0]) {
        case 'v' : // protocol version (mandatory)
          protocolVersion = value.AsInteger();
          break;

        case 'o' : // owner/creator and session identifier (mandatory)
          ParseOwner(value);
          break;

        case 's' : // session name (mandatory)
          sessionName = value;
          break;

        case 'c' : // connection information - not required if included in all media
          defaultConnectAddress = ParseConnectAddress(value);
          break;

        case 't' : // time the session is active (mandatory)
        case 'i' : // session information
        case 'u' : // URI of description
        case 'e' : // email address
        case 'p' : // phone number
          break;
        case 'b' : // bandwidth information
          bandwidth.Parse(value);
          break;
        case 'z' : // time zone adjustments
        case 'k' : // encryption key
        case 'r' : // zero or more repeat times
          break;
        case 'a' : // zero or more session attribute lines
          if (value *= "sendonly")
            SetDirection (SDPMediaDescription::SendOnly);
          else if (value *= "recvonly")
            SetDirection (SDPMediaDescription::RecvOnly);
          else if (value *= "sendrecv")
            SetDirection (SDPMediaDescription::SendRecv);
          else if (value *= "inactive")
            SetDirection (SDPMediaDescription::Inactive);
          break;

        case 'm' : // media name and transport address (mandatory)
          {
            if (currentMedia != NULL) {
              PTRACE(3, "S3\tParsed media session with " << currentMedia->GetSDPMediaFormats().GetSize()
                                                          << " '" << currentMedia->GetSDPMediaType() << "' formats");

// S3: call to opal media format subsystem was removed here, 
// S3: but we need to support absent rtpmaps which was a part of removed post-decoding.
// S3: So leaving it here but overloading PostDecode

              if (!currentMedia->PostDecode(mediaFormats))
                ok = false;

            }

//      OpalMediaTypeDefinition * defn;

            OpalMediaType mediaType;
            currentMedia = NULL;
      PStringArray tokens = value.Tokenise(" ");
      if (tokens.GetSize() < 4) {
        PTRACE(1, "S3\tMedia session has only " << tokens.GetSize() << " elements");
      }
            else 
        if ((mediaType = OpalMediaType::GetMediaTypeFromSDP(tokens[0], tokens[2])).empty()) {
                  PTRACE(1, "S3\tUnknown SDP media type " << tokens[0]);
              }
              else {
// S3: Replacing creation of SDP media description
/*          if ((defn = mediaType.GetDefinition()) == NULL) {
                    PTRACE(1, "S3\tNo definition for SDP media type " << tokens[0]);
                }
                else {
            if ((currentMedia = defn->CreateSDPMediaDescription(defaultConnectAddress)) == NULL) {
                      PTRACE(1, "S3\tCould not create SDP media description for SDP media type " << tokens[0]);
                  } */
            if(mediaType == OpalMediaType::Audio()) {
              currentMedia = new S3SDPAudioMediaDescription(defaultConnectAddress); 
            }
            else {
              if (mediaType == OpalMediaType::Video()) {
                currentMedia = new S3SDPVideoMediaDescription(defaultConnectAddress);
              }
            } 
          }

          if (currentMedia  == NULL) {
                    PTRACE(1, "S3\tCould not create SDP media description for SDP media type " << tokens[0]);
          } 
                else 
            if (currentMedia->Decode(tokens))
                    atLeastOneValidMedia = true;
                  else {
                      delete currentMedia;
                      currentMedia = NULL;
                  }

            if (currentMedia == NULL)
        currentMedia = new SDPDummyMediaDescription(defaultConnectAddress, tokens);

            mediaDescriptions.Append(currentMedia);


          } // case 'm'
          break;

        default:
          PTRACE(1, "S3\tUnknown session information key " << line[0]);
      }
    }
  }

  if (currentMedia != NULL) {
    PTRACE(3, "S3\tParsed final media session with " << currentMedia->GetSDPMediaFormats().GetSize()
                                                << " '" << currentMedia->GetSDPMediaType() << "' formats");

// S3: same as above
  if (!currentMedia->PostDecode(mediaFormats))
    ok = false;
  }

  return ok && (atLeastOneValidMedia || mediaDescriptions.IsEmpty());
}




//                         CreateSDPMediaFormat

S3SDPMediaFormat * S3SDPVideoMediaDescription::CreateSDPMediaFormat(const PString & portString) 
{
    PTRACE(5, "S3\t->S3SDPVideoMediaDescription::CreateSDPMediaFormat");

    return new S3SDPMediaFormat(*this, 
                                (RTP_DataFrame::PayloadTypes)portString.AsUnsigned(), 
                                portString.AsUnsigned(), 
                                portString);
}




//                         CreateSDPMediaFormat

S3SDPMediaFormat * S3SDPAudioMediaDescription::CreateSDPMediaFormat(const PString & portString) 
{
  PTRACE(5, "S3\t->S3SDPAudioMediaDescription::CreateSDPMediaFormat");

  return new S3SDPMediaFormat(*this, 
                              (RTP_DataFrame::PayloadTypes)portString.AsUnsigned(), 
                              portString.AsUnsigned(), 
                              portString);
}




//                     s3PostDecode

bool S3SDPMediaFormat::s3PostDecode(const OpalMediaFormatList & mediaFormats) 
{
    PTRACE(5, "S3\t->S3SDPMediaFormat::s3PostDecode");

    if (encodingName.IsEmpty())
    switch (payloadNumber) 
    {
        case 0 : case 8 : case 9 :
            encodingName = PayloadTypesNames[payloadNumber];
            break;
        default :
            if (m_parent.GetMediaType() == OpalMediaType::Video())
                encodingName = "H264"; // To refactor!!!
            else 
            return false;
            break;
    }
    return true;
}




//                   PostDecode

bool S3SDPVideoMediaDescription::PostDecode(const OpalMediaFormatList & mediaFormats) 
{
    PTRACE(5, "S3\tS3SDPVideoMediaDescription::PostDecode");

/*  unsigned bw = bandwidth[SDPSessionDescription::TransportIndependentBandwidthType()];
    if (bw == 0)
        bw = bandwidth[SDPSessionDescription::ApplicationSpecificBandwidthType()]*1000; */

    S3SDPMediaFormat * format = NULL;
    SDPMediaFormatList::iterator opalFormat = formats.begin();
    while (opalFormat != formats.end()) 
    {
        format = static_cast<S3SDPMediaFormat*>(&(*opalFormat));
        if (format->s3PostDecode(mediaFormats))
            ++opalFormat;
        else
            formats.erase(opalFormat++);
    }
    return true;
}




//                   PostDecode

bool S3SDPAudioMediaDescription::PostDecode(const OpalMediaFormatList & mediaFormats) 
{
    PTRACE(5, "S3\t->S3SDPAudioMediaDescription::PostDecode");

/*  unsigned bw = bandwidth[SDPSessionDescription::TransportIndependentBandwidthType()];
    if (bw == 0)
        bw = bandwidth[SDPSessionDescription::ApplicationSpecificBandwidthType()]*1000;*/

    S3SDPMediaFormat * format = NULL;
    SDPMediaFormatList::iterator opalFormat = formats.begin();
    while (opalFormat != formats.end()) 
    {
        format = static_cast<S3SDPMediaFormat*>(&(*opalFormat));
        if (format->s3PostDecode(mediaFormats))
            ++opalFormat;
        else
            formats.erase(opalFormat++);
    }
    return true;
}
