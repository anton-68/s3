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
* @file s3_h323_connection.cpp
* @brief Overwritten OPAL functionality, see comments for detailes below
* @author Anton Bondarenko
* @version 1
* @date 2012-08-25
*/

#include "s3_h323_connection.h"
#include "s3_call.h"
#include "s3_opal_h323_cxx_inc.h"



//          S3H323Connection

S3H323Connection::S3H323Connection( OpalCall & call,
                                    H323EndPoint & ep,
                                    const PString & token,
                                    const PString & alias,
                                    const H323TransportAddress & address,
                                    unsigned options,
                                    OpalConnection::StringOptions * stringOptions)
                                        : H323Connection(call, ep, token, alias, address, 
                             options, stringOptions){

  PTRACE(5, "S3\tS3 H.323 connection created");
}




//               OnReceivedSignalSetup

PBoolean S3H323Connection::OnReceivedSignalSetup( const H323SignalPDU & originalSetupPDU) {

  if (originalSetupPDU.m_h323_uu_pdu.m_h323_message_body.GetTag() != H225_H323_UU_PDU_h323_message_body::e_setup)
    return PFalse;

  SetPhase(SetUpPhase);

  setupPDU = new H323SignalPDU(originalSetupPDU);
  H225_Setup_UUIE & setup = setupPDU->m_h323_uu_pdu.m_h323_message_body;

  switch (setup.m_conferenceGoal.GetTag()) {
    case H225_Setup_UUIE_conferenceGoal::e_create:
    case H225_Setup_UUIE_conferenceGoal::e_join:
      break;

    case H225_Setup_UUIE_conferenceGoal::e_invite:
      return endpoint.OnConferenceInvite(*setupPDU);

    case H225_Setup_UUIE_conferenceGoal::e_callIndependentSupplementaryService:
      return endpoint.OnCallIndependentSupplementaryService(*setupPDU);

    case H225_Setup_UUIE_conferenceGoal::e_capability_negotiation:
      return endpoint.OnNegotiateConferenceCapabilities(*setupPDU);
  }

  SetRemoteVersions(setup.m_protocolIdentifier);

  // Get the ring pattern
  distinctiveRing = setupPDU->GetDistinctiveRing();

  // Save the identifiers sent by caller
  if (setup.HasOptionalField(H225_Setup_UUIE::e_callIdentifier))
    callIdentifier = setup.m_callIdentifier.m_guid;
  conferenceIdentifier = setup.m_conferenceID;
  SetRemoteApplication(setup.m_sourceInfo);

  // Determine the remote parties name/number/address as best we can
  if (!setupPDU->GetQ931().GetCallingPartyNumber(remotePartyNumber))
    remotePartyNumber = H323GetAliasAddressE164(setup.m_sourceAddress);
  remotePartyName = setupPDU->GetSourceAliases(signallingChannel);

  // get the destination number and name, just in case we are a gateway
  setupPDU->GetQ931().GetCalledPartyNumber(m_calledPartyNumber);
  if (m_calledPartyNumber.IsEmpty())
    m_calledPartyNumber = H323GetAliasAddressE164(setup.m_destinationAddress);

  for (PINDEX i = 0; i < setup.m_destinationAddress.GetSize(); ++i) {
    PString addr = H323GetAliasAddressString(setup.m_destinationAddress[i]);
    if (addr != m_calledPartyNumber) {
      m_calledPartyName = addr;
      break;
    }
  }

  setupPDU->GetQ931().GetRedirectingNumber(m_redirectingParty);

  // get the peer address
  remotePartyAddress = signallingChannel->GetRemoteAddress();
  if (setup.m_sourceAddress.GetSize() > 0)
    remotePartyAddress = H323GetAliasAddressString(setup.m_sourceAddress[0]) + '@' + 
             signallingChannel->GetRemoteAddress();

  // compare the source call signalling address
  if (setup.HasOptionalField(H225_Setup_UUIE::e_sourceCallSignalAddress)) {

    // get the address that remote end *thinks* it is using from the 
  // sourceCallSignalAddress field
    PIPSocket::Address sigAddr;
    {
      H323TransportAddress sigAddress(setup.m_sourceCallSignalAddress);
      sigAddress.GetIpAddress(sigAddr);
    }

    // get the local and peer transport addresses
    PIPSocket::Address peerAddr, localAddr;
    signallingChannel->GetRemoteAddress().GetIpAddress(peerAddr);
    signallingChannel->GetLocalAddress().GetIpAddress(localAddr);

    // allow the application to determine if RTP NAT is enabled or not
    remoteIsNAT = IsRTPNATEnabled(localAddr, peerAddr, sigAddr, PTrue);
  }

  // Anything else we need from setup PDU
  mediaWaitForConnect = setup.m_mediaWaitForConnect;
  if (!setupPDU->GetQ931().GetCalledPartyNumber(localDestinationAddress)) {
    localDestinationAddress = setupPDU->GetDestinationAlias(PTrue);
    if (signallingChannel->GetLocalAddress().IsEquivalent(localDestinationAddress))
      localDestinationAddress = '*';
  }
  
#if OPAL_H460
  ReceiveSetupFeatureSet(this, setup);
#endif

  // Send back a H323 Call Proceeding PDU in case OnIncomingCall() takes a while
  PTRACE(3, "S3\tH225 Sending call proceeding PDU");
  H323SignalPDU callProceedingPDU;
  H225_CallProceeding_UUIE & callProceeding = callProceedingPDU.BuildCallProceeding(*this);

  if (!isConsultationTransfer) {

    if (OnSendCallProceeding(callProceedingPDU)) {
      if (fastStartState == FastStartDisabled)
        callProceeding.IncludeOptionalField(H225_CallProceeding_UUIE::e_fastConnectRefused);

      if (!WriteSignalPDU(callProceedingPDU))
        return PFalse;

      if (GetPhase() < ProceedingPhase) {
        SetPhase(ProceedingPhase);
        OnProceeding();
      }
    }

    /** Here is a spot where we should wait in case of Call Intrusion
    for CIPL from other endpoints 
    if (isCallIntrusion) return PTrue;
    */

    // if the application indicates not to contine, then send a Q931 Release Complete PDU
    alertingPDU = new H323SignalPDU;
    alertingPDU->BuildAlerting(*this);

    /** If we have a case of incoming call intrusion we should not Clear the Call*/
    if (!OnIncomingCall(*setupPDU, *alertingPDU)
#if OPAL_H450
        && !isCallIntrusion
#endif
        ) {
      Release(EndedByNoAccept);
      PTRACE(2, "S3\tH225 Application not accepting calls");
      return PFalse;
    }
    if (IsReleased()) {
      PTRACE(1, "S3\tH225 Application called ClearCall during OnIncomingCall");
      return PFalse;
    }

    // send Q931 Alerting PDU
    PTRACE(3, "S3\tH225 Incoming call accepted");

    // Check for gatekeeper and do admission check if have one
    H323Gatekeeper * gatekeeper = endpoint.GetGatekeeper();
    if (gatekeeper != NULL) {
      H225_ArrayOf_AliasAddress destExtraCallInfoArray;
      H323Gatekeeper::AdmissionResponse response;
      response.destExtraCallInfo = &destExtraCallInfoArray;
      if (!gatekeeper->AdmissionRequest(*this, response)) {
        PTRACE(2, "S3\tH225 Gatekeeper refused admission: "
               << (response.rejectReason == UINT_MAX
                    ? PString("Transport error")
                    : H225_AdmissionRejectReason(response.rejectReason).GetTagName()));
        switch (response.rejectReason) {
          case H225_AdmissionRejectReason::e_calledPartyNotRegistered :
            Release(EndedByNoUser);
            break;
          case H225_AdmissionRejectReason::e_requestDenied :
            Release(EndedByNoBandwidth);
            break;
          case H225_AdmissionRejectReason::e_invalidPermission :
          case H225_AdmissionRejectReason::e_securityDenial :
            ClearCall(EndedBySecurityDenial);
            break;
          case H225_AdmissionRejectReason::e_resourceUnavailable :
            Release(EndedByRemoteBusy);
            break;
          default :
            Release(EndedByGkAdmissionFailed);
        }
        return PFalse;
      }

      if (destExtraCallInfoArray.GetSize() > 0)
        destExtraCallInfo = H323GetAliasAddressString(destExtraCallInfoArray[0]);
      mustSendDRQ = PTrue;
      gatekeeperRouted = response.gatekeeperRouted;
    }
  }

  OnApplyStringOptions();

  // Get the local capabilities before fast start or tunnelled TCS is handled
// S3: Removing this call to prevent both legs from hunging with empty capabilities
// OnSetLocalCapabilities();

  if (fastStartState != FastStartDisabled && 
    setup.HasOptionalField(H225_Setup_UUIE::e_fastStart)) {
    PTRACE(3, "S3\tH225 Fast start detected");
      fastStartState = FastStartDisabled;

// S3: Removing this also:
    // If we have not received caps from remote, we are going to build a
    // fake one from the fast connect data.
// S3:    if (!capabilityExchangeProcedure->HasReceivedCapabilities())
// S3:      remoteCapabilities.RemoveAll();


    // Extract capabilities from the fast start OpenLogicalChannel structures
    if (!processFastStartOffer(setup.m_fastStart)) {
      PTRACE(1, "S3\tProcessing of H323 fast start offer failed");
      return PFalse;
    }
  }

  // Check that it has the H.245 channel connection info
  if (setup.HasOptionalField(H225_Setup_UUIE::e_h245Address) && 
     (!setupPDU->m_h323_uu_pdu.m_h245Tunneling || endpoint.IsH245TunnelingDisabled()))
      if (!CreateOutgoingControlChannel(setup.m_h245Address))
          return PFalse;

  // Build the reply with the channels we are actually using
  connectPDU = new H323SignalPDU;
  connectPDU->BuildConnect(*this);
  
    progressPDU = new H323SignalPDU;
    progressPDU->BuildProgress(*this);

    connectionState = AwaitingLocalAnswer;

    PTRACE(4, "S3\tCalling OnSetup on OpalCall");
    ownerCall.OnSetUp(*this);

    if (connectionState == ShuttingDownConnection)
      return false;

    if (connectionState != AwaitingLocalAnswer)
      return true;

#if OPAL_H450
  // If Call Intrusion is allowed we must answer the call
  if (IsCallIntrusion())
    AnsweringCall(AnswerCallDeferred);
  else if (isConsultationTransfer)
    AnsweringCall(AnswerCallNow);
  else
#endif

    // call the application callback to determine if to answer the call or not
    AnsweringCall(OnAnswerCall(remotePartyName, *setupPDU, *connectPDU, *progressPDU));

    return connectionState != ShuttingDownConnection;
}




//               processFastStartOffer

PBoolean S3H323Connection::processFastStartOffer(const H225_ArrayOf_PASN_OctetString & offer)
{
    PTRACE(4, "S3\tProcessing fast connect offer");
    
    PINDEX i;
    S3MediaDescriptor * descriptor;
    S3Call * call = (S3Call*)(&ownerCall);
    S3H323Connection::ChannelDirection direction;
    S3MediaDescriptor olcDirection;

    for (i = 0; i < offer.GetSize(); i++) 
    {
        H245_OpenLogicalChannel open;
        if (offer[i].DecodeSubType(open)) 
        {
            const H245_DataType * dataType = NULL;
            const H245_H2250LogicalChannelParameters * parameters = NULL;
            if (open.HasOptionalField(H245_OpenLogicalChannel::e_reverseLogicalChannelParameters))
            {
                if (open.m_reverseLogicalChannelParameters.m_multiplexParameters.GetTag()

                    == H245_OpenLogicalChannel_reverseLogicalChannelParameters_multiplexParameters::e_h2250LogicalChannelParameters) 
                {
                    dataType = &open.m_reverseLogicalChannelParameters.m_dataType;
                    parameters = &(const H245_H2250LogicalChannelParameters &)
                    open.m_reverseLogicalChannelParameters.m_multiplexParameters;
                    direction = S3H323Connection::reverse;
                }
            }
            else 
            {
                if (open.m_forwardLogicalChannelParameters.m_multiplexParameters.GetTag() 
                    == H245_OpenLogicalChannel_forwardLogicalChannelParameters_multiplexParameters::e_h2250LogicalChannelParameters)

                {
                    dataType = &open.m_forwardLogicalChannelParameters.m_dataType;
                    parameters = &(const H245_H2250LogicalChannelParameters &)
                    open.m_forwardLogicalChannelParameters.m_multiplexParameters;
                    direction = S3H323Connection::forward;
                }
            }
            if (dataType != NULL) 
            {
                switch (dataType->GetTag()) 
                {
                    case H245_DataType::e_audioData : case H245_DataType::e_videoData :
                        descriptor = call->getDescriptorFactory()->createDescriptor(*dataType, *parameters, direction, S3MediaDescriptor::originator);
                        if(descriptor != NULL) 
                        {
                            descriptor->setOLCNumber(direction, i);
                            if(call->addDescriptor(*descriptor, S3MediaDescriptor::originator)) 
                                PTRACE(5, "S3\tCapability has been added successfully: " << *descriptor);
                            else
                                delete descriptor;
                        }
                        break;
                    case H245_DataType::e_data : default :
                        break;
                }
            }
        }
    }
    if(call->getMediaDescriptorList(S3MediaDescriptor::originator)->GetSize() == 0)
        return false;
    else 
    {
        fastStartState = FastStartResponse;
        return true;  
    }
}





//               SetConnected

PBoolean S3H323Connection::SetConnected() {
  PSafeLockReadWrite safeLock(*this);
  if (!safeLock.IsLocked() || GetPhase() >= ConnectedPhase)
    return PFalse;

  mediaWaitForConnect = PFalse;

  PTRACE(3, "S3\tH323 SetConnected " << *this);
  if (connectPDU == NULL){
    PTRACE(1, "S3\tH323 SetConnected connectPDU is null" << *this);
    return PFalse;
  }  

  if (!endpoint.OnSendConnect(*this, *connectPDU)){
    /* let the application to avoid sending the connect, mainly for testing other endpoints*/
    PTRACE(2, "S3\tH323 SetConnected connect not sent");
    return PTrue;
  }  

// S3: Avoiding empty initial local set
// Assure capabilities are set to other connections media list (if not already)
// OnSetLocalCapabilities();

  H225_Connect_UUIE & connect = connectPDU->m_h323_uu_pdu.m_h323_message_body;

  // Now ask the application to select which channels to start
  if (SendFastStartAcknowledge(connect.m_fastStart)){
    PTRACE(5, "S3\tconnect.m_fastStart = " << connect.m_fastStart);
      connect.IncludeOptionalField(H225_Connect_UUIE::e_fastStart);
  }

  // See if aborted call
  if (connectionState == ShuttingDownConnection)
    return PFalse;

  // Set flag that we are up to CONNECT stage
  connectionState = HasExecutedSignalConnect;
  SetPhase(ConnectedPhase);

#if OPAL_H450
  h450dispatcher->AttachToConnect(*connectPDU);
#endif

  if (!endpoint.IsH245Disabled()){
    if (h245Tunneling) {
      HandleTunnelPDU(connectPDU);
  
      // If no channels selected (or never provided) do traditional H245 start
      if (fastStartState == FastStartDisabled) {
        h245TunnelTxPDU = connectPDU; // Piggy back H245 on this reply
        PBoolean ok = StartControlNegotiations();
        h245TunnelTxPDU = NULL;
        if (!ok)
          return PFalse;
      }
    }
    else if (!controlChannel) { // Start separate H.245 channel if not tunneling.
      if (!CreateIncomingControlChannel(connect.m_h245Address))
        return PFalse;
      connect.IncludeOptionalField(H225_Connect_UUIE::e_h245Address);
    }
  }

  if (!WriteSignalPDU(*connectPDU)) // Send H323 Connect PDU
    return PFalse;

  delete connectPDU;
  connectPDU = NULL;
  delete alertingPDU;
  alertingPDU = NULL;

  InternalEstablishedConnectionCheck();
  return PTrue;
}





//               SendFastStartAcknowledge

PBoolean S3H323Connection::SendFastStartAcknowledge(H225_ArrayOf_PASN_OctetString & fastStartReply) {

  if (fastStartReply.GetSize() > 0) // See if we have already added the fast start OLC's
    return true;

  if (fastStartState == FastStartDisabled)
    return false;

  if (fastStartState == FastStartAcknowledged)
    return true;

  H245_OpenLogicalChannel open;

  if (fastStartChannels.IsEmpty()) {  // See if remote endpoint wants to start fast
    H225_Setup_UUIE & setup = setupPDU->m_h323_uu_pdu.m_h323_message_body;
    if (setup.HasOptionalField(H225_Setup_UUIE::e_fastStart)) {
      
      S3MediaDescriptor * s3Descriptor[2];

      unsigned numberOfMedia = 1;
      s3Descriptor[0] = static_cast<S3Call*>(&ownerCall)->getSelectedDescriptor(S3MediaDescriptor::audio);

      PBoolean videoIsPresent = !(static_cast<S3Call*>(&ownerCall)->getSelectedDescriptor(S3MediaDescriptor::video))->isEmpty();

      if(videoIsPresent) {
        s3Descriptor[1] = static_cast<S3Call*>(&ownerCall)->getSelectedDescriptor(S3MediaDescriptor::video);
        numberOfMedia = 2;
      }
      
      for(unsigned i = 0; i < numberOfMedia; i++) {

        if (setup.m_fastStart[s3Descriptor[i]->getOLCNumber(S3H323Connection::reverse)].DecodeSubType(open)) {
          unsigned error;
          H323Channel * channel = CreateLogicalChannel(open, PTrue, error, s3Descriptor[i]);
          if (channel != NULL) {
            channel->SetNumber(logicalChannels->GetNextChannelNumber());
            fastStartChannels.Append(channel);
          }
        }
        else 
          PTRACE(1, "S3\tH225 Invalid fast start PDU decode:\n  " << open);

        if (setup.m_fastStart[s3Descriptor[i]->getOLCNumber(S3H323Connection::forward)].DecodeSubType(open)) {
          unsigned error;
          H323Channel * channel = CreateLogicalChannel(open, PTrue, error, s3Descriptor[i]);
          if (channel != NULL) {
            fastStartChannels.Append(channel);
          }
        }
        else 
          PTRACE(1, "S3\tH225 Invalid fast start PDU decode:\n  " << open);

      }

      // Extract capabilities from the fast start OpenLogicalChannel structures
      // for (PINDEX i = 0; i < setup.m_fastStart.GetSize(); i++) {
      // S3: will go instead through selected channels:
/*      unsigned selectedChannels[] = {0,1,50,51}; // workaround until we learn how to  process media formats on both legs
      for(unsigned i = 0; i < 4; i++) {
        H245_OpenLogicalChannel open;
        if (setup.m_fastStart[selectedChannels[i]].DecodeSubType(open)) {
          PTRACE(5, "S3\tH225 Fast start open:\n  " << setprecision(2) << open);
          unsigned error;
          H323Channel * channel = CreateLogicalChannel(open, PTrue, error, findDescriptor(selectedChannels[i]));
          if (channel != NULL) {
            if (channel->GetDirection() == H323Channel::IsTransmitter)
              channel->SetNumber(logicalChannels->GetNextChannelNumber());
            fastStartChannels.Append(channel);
          }
        }
        else 
          PTRACE(1, "S3\tH225 Invalid fast start PDU decode:\n  " << open);
      } */

      PTRACE(3, "S3\tH225 Opened " << fastStartChannels.GetSize() << " fast start channels");

      if (fastStartChannels.IsEmpty()) {
            fastStartState = FastStartDisabled;
            return false;
      }

      fastStartState = FastStartResponse;
    }
  }

  //See if we need to select our fast start channels
  if (fastStartState == FastStartResponse)
    OnSelectLogicalChannels();
          
  // Remove any channels that were not started by OnSelectLogicalChannels(),
  // those that were started are put into the logical channel dictionary
  // S3:
  // for (H323LogicalChannelList::iterator channel = fastStartChannels.begin(); channel != fastStartChannels.end(); ) {
      //if (channel->IsOpen())
          //logicalChannels->Add(*channel++);
      //else
          //fastStartChannels.erase(channel++); // Do ++ in both legs so iterator works with erase
  //}

  // None left, so didn't open any channels fast
  if (fastStartChannels.IsEmpty()) {
    fastStartState = FastStartDisabled;
    return PFalse;
  }

  // The channels we just transferred to the logical channels dictionary
  // should not be deleted via this structure now.
  fastStartChannels.DisallowDeleteObjects();

    PTRACE(3, "S3\tH225 Accepting fastStart for " << fastStartChannels.GetSize() << " channels");

  for (H323LogicalChannelList::iterator channel = fastStartChannels.begin(); channel != fastStartChannels.end(); ++channel) {
    PTRACE(5, "S3\tBuildFastStartList returned " 
      << BuildFastStartList(*channel, fastStartReply, H323Channel::IsTransmitter) 
      << " for channel " << *channel);
  }

  // Have moved open channels to logicalChannels structure, remove all others.
  fastStartChannels.RemoveAll();

  // Set flag so internal establishment check does not require H.245
  fastStartState = FastStartAcknowledged;

  return PTrue;
}




//                CreateLogicalChannel

H323Channel * S3H323Connection::CreateLogicalChannel(const H245_OpenLogicalChannel & open,
                                                     PBoolean startingFast,
                                                     unsigned & errorCode,
                           const S3MediaDescriptor * descriptor) // parameter added within S3
{
  const H245_H2250LogicalChannelParameters * param;
  const H245_DataType * dataType;
  H323Channel::Directions direction;
  H323Capability * capability;

  if (startingFast && open.HasOptionalField(H245_OpenLogicalChannel::e_reverseLogicalChannelParameters)) {
    if (open.m_reverseLogicalChannelParameters.m_multiplexParameters.GetTag() !=
      H245_OpenLogicalChannel_reverseLogicalChannelParameters_multiplexParameters
                                                      ::e_h2250LogicalChannelParameters) {
      errorCode = H245_OpenLogicalChannelReject_cause::e_unsuitableReverseParameters;
      PTRACE(1, "S3\tH323 CreateLogicalChannel - reverse channel, H225.0 only supported");
      return NULL;
    }

    PTRACE(3, "S3\tH323 CreateLogicalChannel - reverse channel");
    dataType = &open.m_reverseLogicalChannelParameters.m_dataType;
    param = &(const H245_H2250LogicalChannelParameters &)
                      open.m_reverseLogicalChannelParameters.m_multiplexParameters;
    direction = H323Channel::IsTransmitter;

// S3:    capability = remoteCapabilities.FindCapability(*dataType);

  }
    else {
    if (open.m_forwardLogicalChannelParameters.m_multiplexParameters.GetTag() !=
      H245_OpenLogicalChannel_forwardLogicalChannelParameters_multiplexParameters
                                                      ::e_h2250LogicalChannelParameters) {
      PTRACE(1, "H323\tCreateLogicalChannel - forward channel, H225.0 only supported");
      errorCode = H245_OpenLogicalChannelReject_cause::e_unspecified;
      return NULL;
    }

    PTRACE(3, "H323\tCreateLogicalChannel - forward channel");
    dataType = &open.m_forwardLogicalChannelParameters.m_dataType;
    param = &(const H245_H2250LogicalChannelParameters &)
        open.m_forwardLogicalChannelParameters.m_multiplexParameters;
    direction = H323Channel::IsReceiver;

    PString mediaPacketization;
    if (param->HasOptionalField(H245_H2250LogicalChannelParameters::e_mediaPacketization) &&
      param->m_mediaPacketization.GetTag() == H245_H2250LogicalChannelParameters_mediaPacketization::e_rtpPayloadType)
          mediaPacketization = H323GetRTPPacketization(param->m_mediaPacketization);

// S3: capability = localCapabilities.FindCapability(*dataType, mediaPacketization);

  }

// S3: Here we create capability our way:
  capability = descriptor->getOpalCapability();

/*  if (capability == NULL) {
    errorCode = H245_OpenLogicalChannelReject_cause::e_unknownDataType;
    PTRACE(1, "H323\tCreateLogicalChannel - unknown data type");
    return NULL; // If codec not supported, return error
  } */

  if (!capability->OnReceivedPDU(*dataType, direction == H323Channel::IsReceiver)) {
      errorCode = H245_OpenLogicalChannelReject_cause::e_dataTypeNotSupported;
      PTRACE(1, "H323\tCreateLogicalChannel - data type not supported");
      return NULL; // If codec not supported, return error
  } 

  //capability->OnReceivedPDU(*dataType, direction);
  PTRACE(5, "S3\tS3 H323 capability created");
  PTRACE(5, "S3\t" << setprecision(2) << *capability);

// there are some doubts that this shoud be left here,,,
  if (!OnCreateLogicalChannel(*capability, direction, errorCode))
      return NULL; // If codec combination not supported, return error
  PTRACE(5, "S3\tS3 OnCreateLogicalChannel completed");

//  H323Channel * channel = H323Capability::CreateChannel(*this, direction, param->m_sessionID, param); 
  H323Channel * channel = capability->CreateChannel(*this, direction, param->m_sessionID, param);

  PTRACE(5, "S3\tChannel created from capability: " << *channel);

  if (channel == NULL) {
    errorCode = H245_OpenLogicalChannelReject_cause::e_dataTypeNotAvailable;
    PTRACE(1, "S3\tH323 CreateLogicalChannel - data type not available");
    return NULL;
  }

  H323TransportAddress * data, * ctrl;
  PIPSocket::Address addr;
  WORD port;

  if(descriptor->getMediaIP(S3MediaDescriptor::terminator).GetIpAndPort(addr, port)) {
    PTRACE(1, "S3\tUnable to extract transport address");
  }

  data = new H323TransportAddress(addr, port);
  ctrl = new H323TransportAddress(addr, port+1);

  static_cast<H323_ExternalRTPChannel*>(channel)->SetExternalAddress(*data, *ctrl);
  static_cast<H323_ExternalRTPChannel*>(channel)->SetDynamicRTPPayloadType((int)descriptor->getPayloadType());

/*  if (!channel->SetInitialBandwidth())
      errorCode = H245_OpenLogicalChannelReject_cause::e_insufficientBandwidth;
    else 
    if (channel->OnReceivedPDU(open, errorCode))*/
        return channel;

/*  PTRACE(1, "H323\tOnReceivedPDU gave error " << errorCode);
  delete channel;
  return NULL;*/
}




// CreateRealTimeLogicalChannel

H323Channel * S3H323Connection::CreateRealTimeLogicalChannel(const H323Capability & capability,
                                                             H323Channel::Directions dir,
                                                             unsigned sessionID,
                                                         const H245_H2250LogicalChannelParameters * param,
                                                             RTP_QOS * rtpqos) {

  OpalMediaType mediaType = capability.GetMediaFormat().GetMediaType();

  if (sessionID == 0)
    sessionID = GetNextSessionID(mediaType, true);

  {
    PSafeLockReadOnly m(ownerCall);

    if (ownerCall.IsMediaBypassPossible(*this, sessionID)) {
      PSafePtr<OpalRTPConnection> otherParty = GetOtherPartyConnectionAs<OpalRTPConnection>();
      if (otherParty == NULL) {
        PTRACE(1, "H323\tCowardly refusing to create an RTP channel with only one connection");
      return NULL;
    }

    //MediaInformation info;
    //if (otherParty->GetMediaInformation(sessionID, info))
    //PIPSocket::Address addr(4, getMediaIP[S3MediaDescriptor::terminator]);
    //H323Transport Address h323Addr(addr, getMediaPort[S3MediaDescriptor::terminator]);    
    //H323Transport Address h323CtrlAddr(addr, getMediaPort[S3MediaDescriptor::terminator]+1);    

    return new H323_ExternalRTPChannel(*this, capability, dir, sessionID /*, addr*/ /*info.data*/ /*, addr*/ /*info.control*/);
    PTRACE(5, "S3\tCapbality created sccessfully");
      //return new S3H323ExternalRTPChannel(*this, capability, dir, sessionID);
    }
  }

  RTP_Session * session;

  if (param != NULL && param->HasOptionalField(H245_H2250LogicalChannelParameters::e_mediaControlChannel)) {
    // We only support unicast IP at this time.
    if (param->m_mediaControlChannel.GetTag() != H245_TransportAddress::e_unicastAddress)
      return NULL;

    const H245_UnicastAddress & uaddr = param->m_mediaControlChannel;
    unsigned int tag = uaddr.GetTag();
    if ((tag != H245_UnicastAddress::e_iPAddress) && (tag != H245_UnicastAddress::e_iP6Address))
      return NULL;
  }

  session = UseSession(GetControlChannel(), sessionID, mediaType, rtpqos);
  if (session == NULL)
    return NULL;

  ((RTP_UDP *) session)->Reopen(dir == H323Channel::IsReceiver);
  return CreateRTPChannel(capability, dir, *session);
}




//               OnCreateLogicalChannel

PBoolean S3H323Connection::OnCreateLogicalChannel(const H323Capability & capability,
                                                H323Channel::Directions dir,
                                                unsigned & errorCode) 
{
  if (connectionState == ShuttingDownConnection) 
  {
    errorCode = H245_OpenLogicalChannelReject_cause::e_unspecified;
    return PFalse;
  }

  // Default error if returns PFalse
  errorCode = H245_OpenLogicalChannelReject_cause::e_unspecified;

// S3: Checks of capabilities and channel states removed

  return PTrue;
}




//                         IsMediaBypassPossible

PBoolean S3H323Connection::IsMediaBypassPossible(unsigned /*sessionID*/) const 
{
    PTRACE(4, "S3\t H323 connection: IsMediaBypassPossible: true");
    return true;
}
