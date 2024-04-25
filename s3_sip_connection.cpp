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
* @file s3_sip_connection.cpp
* @brief Partially borrowed and changed OPAL code 
* @author Anton Bondarenko
* @version 1
* @date 2012-09-07
*/

#include "s3_sip_connection.h"
#include "s3_manager.h"
#include "s3_media_descriptor.h"
#include "s3_opal_sip_cxx_inc.h"
#include "s3_sdp.h"




//               S3SIPConnection

S3SIPConnection::S3SIPConnection(OpalCall & call,
                              SIPEndPoint & ep,
                            const PString & token,
                             const SIPURL & destination,
                            OpalTransport * transport,
                               unsigned int options,
            OpalConnection::StringOptions * stringOptions)
        : SIPConnection(call, ep, token, destination, transport, options , stringOptions) 
{
    PTRACE(5, "S3\t::S3SIPConnection");

    m_prackMode = e_prackDisabled;
}




//                        SetUpConnection

PBoolean S3SIPConnection::SetUpConnection()
{

    PTRACE(5, "S3\t->S3SIPConnection::SetUpConnection");


  originating = true;

  OnApplyStringOptions();

  if (m_stringOptions.Contains(SIP_HEADER_PREFIX"Route")) {
    SIPMIMEInfo mime;
    mime.SetRoute(m_stringOptions[SIP_HEADER_PREFIX"Route"]);
    m_dialog.SetRouteSet(mime.GetRoute());
  }

  SIPURL transportAddress;

  if (!m_dialog.GetRouteSet().empty()) 
    transportAddress = m_dialog.GetRouteSet().front();
  else if (!m_dialog.GetProxy().IsEmpty())
    transportAddress = m_dialog.GetProxy().GetHostAddress();
  else {
    transportAddress = m_dialog.GetRequestURI();
    transportAddress.AdjustToDNS(); // Do a DNS SRV lookup
    PTRACE(4, "S3\tSIP:Connecting to " << m_dialog.GetRequestURI() << " via " << transportAddress);
  }

  if (!SetTransport(transportAddress)) {
    Release(EndedByUnreachable);
    return false;
  }

  ++m_sdpVersion;

// S3: Remove this call, we're dodging use of OpalMediaFormats mechanics:
//  if (!SetRemoteMediaFormats(NULL))
//    return false;

  bool ok;
  if (!transport->GetInterface().IsEmpty())
    ok = WriteINVITE();
  else {
    PWaitAndSignal mutex(transport->GetWriteMutex());
    m_dialog.SetForking(true);
    ok = transport->WriteConnect(WriteINVITE, this);
    m_dialog.SetForking(false);
  }

  SetPhase(SetUpPhase);

  if (ok) {
    releaseMethod = ReleaseWithCANCEL;
    m_handlingINVITE = true;
    return true;
  }

  PTRACE(1, "S3\tSIP:Could not write to " << transportAddress << " - " << transport->GetErrorText());
  Release(EndedByTransportFail);
  return false;

}




//                    OnCreatingINVITE

void S3SIPConnection::OnCreatingINVITE(SIPInvite & request) 
{
    PTRACE(5, "S3\t->S3SIPConnection::OnCreatingINVITE");


SIPMIMEInfo & mime = request.GetMIME();

  switch (m_prackMode) {
    case e_prackDisabled :
      break;

    case e_prackRequired :
      mime.AddRequire("100rel");
      // Then add supported as well

    case e_prackSupported :
      mime.AddSupported("100rel");
  }

  mime.AddSupported("replaces");
  for (PINDEX i = 0; i < m_stringOptions.GetSize(); ++i) {
    PCaselessString key = m_stringOptions.GetKeyAt(i);
    if (key.NumCompare(HeaderPrefix) == EqualTo) {
      PString data = m_stringOptions.GetDataAt(i);
      if (!data.IsEmpty()) {
        mime.SetAt(key.Mid(HeaderPrefix.GetLength()), m_stringOptions.GetDataAt(i));
        if (key == SIP_HEADER_REPLACES)
          mime.AddRequire("replaces");
      }
    }
  }

  if (IsPresentationBlocked()) {
    // Should do more as per RFC3323, but this is all for now
    SIPURL from = mime.GetFrom();
    if (!from.GetDisplayName(false).IsEmpty())
      from.SetDisplayName("Anonymous");
    mime.SetFrom(from.AsQuotedString());
  }

  PString externalSDP = m_stringOptions(OPAL_OPT_EXTERNAL_SDP);
  if (!externalSDP.IsEmpty())
    request.SetEntityBody(externalSDP);
  else if (m_stringOptions.GetBoolean(OPAL_OPT_INITIAL_OFFER, true)) {
    if (m_needReINVITE)
      ++m_sdpVersion;

//  S3:BEGIN
//  S3:Adding capabilities from a-leg:
    S3SDPMediaFormat * f = NULL;
    SDPSessionDescription * sdp = new SDPSessionDescription(m_sdpSessionId, m_sdpVersion, OpalTransportAddress());
    sdp->SetOwnerSessionId(0);
    sdp->SetOwnerVersion(0);
    //PIPSocket::Address * addr;
    //WORD port;
    OpalTransportAddress * videoTransportAddress = NULL;
    OpalTransportAddress * audioTransportAddress = NULL;
    const S3MediaDescriptorList * descriptorList = static_cast<S3Call*>(&ownerCall)->getMediaDescriptorList(S3MediaDescriptor::originator);

    for(S3MediaDescriptorList::const_iterator i = descriptorList->begin(); i != descriptorList->end(); i++ ){
        if (i->isVideo()) {
            if(videoTransportAddress == NULL) { /*
                addr = new PIPSocket::Address();
                i->getMediaIP(S3MediaDescriptor::originator).GetIpAndPort(*addr, port);
                PTRACE(5, "S3\tStored address: " << i->getMediaIP(S3MediaDescriptor::originator));
                PTRACE(5, "S3\tAssigned transport address for video: " << *addr << ", port: " << port);
                videoTransportAddress = new OpalTransportAddress(
                    *addr, 
                    i->getMediaPort(S3MediaDescriptor::originator), 
                    "udp"); */
            /*  videoTransportAddress = new OpalTransportAddress(
                    PIPSocket::Address(4, i->getMediaIP(S3MediaDescriptor::originator)), 
                    (i->getMediaPort(S3MediaDescriptor::originator)).GetValue(), 
                    "udp");*/
                
                videoTransportAddress = new OpalTransportAddress(i->getMediaIP(S3MediaDescriptor::originator));
            }
        }
        else {
            if (i->isAudio()) { 
                if(audioTransportAddress == NULL) {/*
                    addr = new PIPSocket::Address();
                    i->getMediaIP(S3MediaDescriptor::originator).GetIpAndPort(*addr, port);
                    PTRACE(5, "S3\tAssigned transport address for audio : " << *addr);
                    audioTransportAddress = new OpalTransportAddress(
                    *addr, 
                    i->getMediaPort(S3MediaDescriptor::originator), 
                    "udp");*/
                /*  audioTransportAddress = new OpalTransportAddress(PIPSocket::Address(4, i->getMediaIP(S3MediaDescriptor::originator)), 
                                                    (i->getMediaPort(S3MediaDescriptor::originator)).GetValue(), "udp");*/
                audioTransportAddress = new OpalTransportAddress(i->getMediaIP(S3MediaDescriptor::originator));
                }
            }
            else {
                PTRACE(1, "S3\tWrong mediatype in descriptor: " << endl << (*f));
                return;
            }
        }   
    }
    S3SDPAudioMediaDescription * audioDescriptor = new S3SDPAudioMediaDescription(*audioTransportAddress);
    //audioDescriptor->SetTransportAddress(*audioTransportAddress);
    S3SDPVideoMediaDescription * videoDescriptor = new S3SDPVideoMediaDescription(*videoTransportAddress);
    //videoDescriptor->SetTransportAddress(*videoTransportAddress);
    for(S3MediaDescriptorList::const_iterator i = descriptorList->begin(); i != descriptorList->end(); i++ ){
        if (i->isVideo()) {
            f = i->getSDPFormat(*videoDescriptor);
            //...
            videoDescriptor->AddSDPMediaFormat(f);
        }
        else {
            f = i->getSDPFormat(*audioDescriptor);
            audioDescriptor->AddSDPMediaFormat(f);
        }   
    }

//  sdp->SetOwnerAddress(OpalTransportAddress("udp$192.168.122.75:5060")); // to refactor
// S3: Using first listener local S3 address
    sdp->SetOwnerAddress(endpoint.GetInterfaceAddresses()[0]);
// S3: Not needed cause we provide connection address to each media description
    sdp->SetDefaultConnectAddress(OpalTransportAddress("udp$192.168.0.107:5060")); // to refactor
    sdp->AddMediaDescription(audioDescriptor);
    sdp->AddMediaDescription(videoDescriptor);
    PTRACE(4, "S3\tPrepared SDP: ");
    static_cast<S3SDPSessionDescription*>(sdp)->s3Print(cout);
// S3:END

    if (OnSendOfferSDP(request.GetSessionManager(), *sdp, m_needReINVITE))
      request.SetSDP(sdp);
    else {
      delete sdp;
      Release(EndedByCapabilityExchange);
    }
  }
}




//                    OnSendOfferSDPSession

bool S3SIPConnection::OnSendOfferSDPSession(const OpalMediaType & mediaType,
                                                         unsigned rtpSessionId,
                                          OpalRTPSessionManager & rtpSessions,
                                          SDPSessionDescription & sdp,
                                                             bool offerOpenMediaStreamOnly) 
{
    PTRACE(5, "S3\t->S3SIPConnection::OnSendOfferSDPSession");
    OpalMediaType::AutoStartMode autoStart = GetAutoStart(mediaType);
    if (rtpSessionId == 0 && autoStart == OpalMediaType::DontOffer)
        return false; 
    PTRACE(3, "SIP\tOffering media type " << mediaType << " in SDP");
    return true;

    // S3:SDP construction removed
    
}




//                    OnReceivedResponceToINVITE 

void S3SIPConnection::OnReceivedResponseToINVITE(SIPTransaction & transaction, SIP_PDU & response) 
{

    PTRACE(5, "S3\t->S3SIPConnection::OnReceivedResponseToINVITE");


  unsigned statusCode = response.GetStatusCode();
  unsigned statusClass = statusCode/100;
  if (statusClass > 2)
    return;

  PSafeLockReadWrite lock(*this);
  if (!lock.IsLocked())
    return;

  // See if this is an initial INVITE or a re-INVITE
  bool reInvite = true;
  for (PSafePtr<SIPTransaction> invitation(forkedInvitations, PSafeReference); invitation != NULL; ++invitation) {
    if (invitation == &transaction) {
      reInvite = false;
      break;
    }
  }

  // If we are in a dialog, then m_dialog needs to be updated in the 2xx/1xx
  // response for a target refresh request
  m_dialog.Update(*transport, response);

  const SIPMIMEInfo & responseMIME = response.GetMIME();

  {
    SIPURL newRemotePartyID(responseMIME, RemotePartyID);
    if (!newRemotePartyID.IsEmpty()) {
      if (m_ciscoRemotePartyID.IsEmpty() && newRemotePartyID.GetUserName() == m_dialog.GetRemoteURI().GetUserName()) {
        PTRACE(3, "SIP\tOld style Remote-Party-ID set to \"" << newRemotePartyID << '"');
        m_ciscoRemotePartyID = newRemotePartyID;
      }
      else if (m_ciscoRemotePartyID != newRemotePartyID) {
        PTRACE(3, "SIP\tOld style Remote-Party-ID used for forwarding indication to \"" << newRemotePartyID << '"');

        m_ciscoRemotePartyID = newRemotePartyID;
        newRemotePartyID.SetParameters(PString::Empty());

        PStringToString info = m_ciscoRemotePartyID.GetParamVars();
        info.SetAt("result", "forwarded");
        info.SetAt("party", "A");
        info.SetAt("code", psprintf("%u", statusCode));
        info.SetAt("Referred-By", m_dialog.GetRemoteURI().AsString());
        info.SetAt("Remote-Party", newRemotePartyID.AsString());
        OnTransferNotify(info, this);
      }
    }
  }

  // Update internal variables on remote part names/number/address
  UpdateRemoteAddresses();

  if (reInvite)
    return;

  if (statusClass == 2) {
    // Have a final response to the INVITE, so cancel all the other invitations sent.
    for (PSafePtr<SIPTransaction> invitation(forkedInvitations, PSafeReference); invitation != NULL; ++invitation) {
      if (invitation != &transaction)
        invitation->Cancel();
    }

    // And end connect mode on the transport
    transport->SetInterface(transaction.GetInterface());
    m_contactAddress = transaction.GetMIME().GetContact();
  }

  responseMIME.GetProductInfo(remoteProductInfo);

  // Save the sessions etc we are actually using of all the forked INVITES sent
// S3: Implanting of S3 SDP descriptor
  S3SDPSessionDescription * sdp = static_cast<S3SIP_PDU*>(&response)->GetSDP();
  if (sdp != NULL) {
    m_rtpSessions = ((SIPInvite &)transaction).GetSessionManager();
    if (remoteProductInfo.vendor.IsEmpty() && remoteProductInfo.name.IsEmpty()) {
      if (sdp->GetSessionName() != "-")
        remoteProductInfo.name = sdp->GetSessionName();
      if (sdp->GetUserName() != "-")
        remoteProductInfo.vendor = sdp->GetUserName();
    }
  }

  // Do PRACK after all the dialog completion parts above.
  if (statusCode > 100 && statusCode < 200 && responseMIME.GetRequire().Contains("100rel")) {
    PString rseq = responseMIME.GetString("RSeq");
    if (rseq.IsEmpty()) {
      PTRACE(2, "SIP\tReliable (100rel) response has no RSeq field.");
    }
    else if (rseq.AsUnsigned() <= m_prackSequenceNumber) {
      PTRACE(3, "SIP\tDuplicate response " << response.GetStatusCode() << ", already PRACK'ed");
    }
    else {
      transport->SetInterface(transaction.GetInterface()); // Make sure same as response
      SIPTransaction * prack = new SIPPrack(*this, rseq & transaction.GetMIME().GetCSeq());
      prack->Start();
    }
  }
}





//                    OnReceivedAnswerSDP

void S3SIPConnection::OnReceivedAnswerSDP(SIP_PDU & response) 
{
    PTRACE(5, "S3\t->S3SIPConnection::OnReceivedAnswerSDP");

    SDPSessionDescription * sdp = response.GetSDP(m_localMediaFormats);
// S3: Adding tracing:
    if(sdp == NULL) {
        PTRACE(5, "S3\tOnReceivedAnswerSDP SDP = NULL");
        return;
    }
    else {
        PTRACE(5, "S3\tOnReceivedAnswerSDP SDP: ");
        static_cast<S3SDPSessionDescription*>(sdp)->s3Print(cout);
    }

// S3: Media formats mechanics detached:

/*  m_answerFormatList = sdp->GetMediaFormats();
    AdjustMediaFormats(false, NULL, m_answerFormatList);*/

    m_holdFromRemote = sdp->IsHold();

// S3: Have nohing to do with media streams

/*  unsigned sessionCount = sdp->GetMediaDescriptions().GetSize();
    bool multipleFormats = false;
    bool ok = false;
    for (unsigned session = 1; session <= sessionCount; ++session) {
        if (OnReceivedAnswerSDPSession(*sdp, session, multipleFormats))
            ok = true; 
            else { 
                OpalMediaStreamPtr stream;
                if ((stream = GetMediaStream(session, false)) != NULL)
                    stream->Close();
                if ((stream = GetMediaStream(session, true)) != NULL)
                    stream->Close(); 
            } 
    }

    m_answerFormatList.RemoveAll(); */

/*   Shut down any media that is in a session not mentioned in a re-INVITE.
     While the SIP/SDP specification says this shouldn't happen, it does
     anyway so we need to deal. */

/*
  for (OpalMediaStreamPtr stream(mediaStreams, PSafeReference); stream != NULL; ++stream) {
    if (stream->GetSessionID() > sessionCount)
      stream->Close();
  }*/

// S3: The next comment (and code) perfectly apply to s3 but requires additional analysis

  /* See if remote has answered our offer with multiple possible codecs.
     While this is perfectly legal, and we are supposed to wait for the first
     RTP packet to arrive before setting up codecs etc, our architecture
     cannot deal with that. So what we do is immediately, send a re-INVITE
     nailing the codec down to the first reply. */

/*if (multipleFormats && m_resolveMultipleFormatReINVITE) {
    m_resolveMultipleFormatReINVITE= false;
    SendReINVITE(PTRACE_PARAM("resolve multiple codecs in answer"));
  }

  if (GetPhase() == EstablishedPhase)
    ownerCall.StartMediaStreams(); // re-INVITE
  else {
    if (!ok)
      Release(EndedByCapabilityExchange);
  } */
// S3: Finally we provide S#-specific SDP-answer handling:
    if(!processSDPAnswer(*static_cast<const S3SDPSessionDescription*>(sdp))) {
        PTRACE(1, "S3\tADP answer processing failed");
    }

}





//                        IsMediaBypassPossible

PBoolean S3SIPConnection::IsMediaBypassPossible(unsigned /*sessionID*/) const 
{
    PTRACE(4, "S3\tSIP connection IsMediaBypassPossible returns true");
    return true;
}




//                        processSDPAnswer

PBoolean S3SIPConnection::processSDPAnswer(const S3SDPSessionDescription & sdp) 
{
    PTRACE(5, "S3\t->S3SIPConnection::processSDPAnswer");

    S3Call * call = static_cast<S3Call*>(&ownerCall);
    S3DescriptorFactory * factory = call->getDescriptorFactory();
    SDPMediaFormatList formats;
    SDPMediaDescriptionArray mediaDescriptions = sdp.GetMediaDescriptions();
    SDPMediaFormatList::iterator format;
    S3MediaDescriptor * descriptor;
    SDPMediaDescription::Direction dir; 
    OpalTransportAddress addr;
    PIPSocket::Address ip;
    WORD port;

    for(PINDEX i = 0; i < mediaDescriptions.GetSize(); i++) {
        if (!mediaDescriptions[i].GetTransportAddress().IsEmpty())
            addr = mediaDescriptions[i].GetTransportAddress();
        else
            addr = sdp.GetDefaultConnectAddress();

        if(mediaDescriptions[i].GetDirection() != SDPMediaDescription::Undefined)
            dir = mediaDescriptions[i].GetDirection();
        else
            dir = sdp.GetDirection();

        addr.GetIpAddress(ip);
        port = mediaDescriptions[i].GetPort();
        addr = OpalTransportAddress(ip, port, "udp");

        formats = mediaDescriptions[i].GetSDPMediaFormats();
        for (format = formats.begin(); format != formats.end(); ++format) {
            descriptor = factory->createDescriptor(mediaDescriptions[i].GetMediaType(), 
                                                   *format,
                                                   addr,
                                                   port,
                                                   dir,
                                                   S3MediaDescriptor::terminator);
        
            if(descriptor != NULL) 
                if(call->addDescriptor(*descriptor, S3MediaDescriptor::terminator)) {
                    PTRACE(5, "S3\t[]processSDPAnswer: descriptor added: " << endl);
                    descriptor->s3print(cout);
                }
                else {
                    PTRACE(2, "S3\t[]processSDPAnswer S3Call::addDescriptor failed");
                    return false;
                }
            else {
                PTRACE(2, "S3\t[]processSDPAnswer received empty descriptor from factory");
                return false;
            }
        }
    }   
    return true;
}
