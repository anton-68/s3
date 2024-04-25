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
* @file s3_opal_h323_cxx_inc.h
* @brief Static non-member code borrowed from OPAL 
* @author Anton Bondarenko
* @version 1
* @date 2012-08-26
*/

#ifndef S3_OPAL_H323_CXX_CONNECTION_H
#define S3_OPAL_H323_CXX_CONNECTION_H

#include <opal/buildopts.h>

#ifdef __GNUC__
#pragma implementation "h323con.h"
#endif

#include <opal/buildopts.h>

#include <h323/h323con.h>

#include <h323/h323ep.h>
#include <h323/h323neg.h>
#include <h323/h323rtp.h>
#include <h323/gkclient.h>

#if OPAL_H450
#include <h323/h450pdu.h>
#endif

#include <h323/transaddr.h>
#include <opal/call.h>
#include <opal/patch.h>
#include <codec/rfc2833.h>

#if OPAL_VIDEO
#include <codec/vidcodec.h>
#endif

//#if OPAL_HAS_H224
//#include <h224/h224.h>
//#endif

#if OPAL_H460
#include <h460/h460.h>
#include <h460/h4601.h>
#endif

const PTimeInterval MonitorCallStatusTime(0, 10); // Seconds

//#if OPAL_H239
//static const PString & H239MessageOID = "0.0.8.239.2";
//#endif

#define new PNEW


/////////////////////////////////////////////////////////////////////////////
/*
#if PTRACING 
const char * H323Connection::GetConnectionStatesName(ConnectionStates s)
{
  static const char * const names[NumConnectionStates] = {
    "NoConnectionActive",
    "AwaitingGatekeeperAdmission",
    "AwaitingTransportConnect",
    "AwaitingSignalConnect",
    "AwaitingLocalAnswer",
    "HasExecutedSignalConnect",
    "EstablishedConnection",
    "ShuttingDownConnection"
  };
  return s < PARRAYSIZE(names) ? names[s] : "<Unknown>";
}

const char * H323Connection::GetFastStartStateName(FastStartStates s)
{
  static const char * const names[NumFastStartStates] = {
    "FastStartDisabled",
    "FastStartInitiate",
    "FastStartResponse",
    "FastStartAcknowledged"
  };
  return s < PARRAYSIZE(names) ? names[s] : "<Unknown>";
}
#endif // PTRACING
*/
#if OPAL_H460
static void ReceiveSetupFeatureSet(const H323Connection * connection, const H225_Setup_UUIE & pdu)
{
  H225_FeatureSet fs;
  PBoolean hasFeaturePDU = PFalse;
  
  if(pdu.HasOptionalField(H225_Setup_UUIE::e_neededFeatures)) {
    fs.IncludeOptionalField(H225_FeatureSet::e_neededFeatures);
    H225_ArrayOf_FeatureDescriptor & fsn = fs.m_neededFeatures;
    fsn = pdu.m_neededFeatures;
    hasFeaturePDU = PTrue;
  }
  
  if(pdu.HasOptionalField(H225_Setup_UUIE::e_desiredFeatures)) {
    fs.IncludeOptionalField(H225_FeatureSet::e_desiredFeatures);
    H225_ArrayOf_FeatureDescriptor & fsn = fs.m_desiredFeatures;
    fsn = pdu.m_desiredFeatures;
    hasFeaturePDU = PTrue;
  }
  
  if(pdu.HasOptionalField(H225_Setup_UUIE::e_supportedFeatures)) {
    fs.IncludeOptionalField(H225_FeatureSet::e_supportedFeatures);
    H225_ArrayOf_FeatureDescriptor & fsn = fs.m_supportedFeatures;
    fsn = pdu.m_supportedFeatures;
    hasFeaturePDU = PTrue;
  }
  
  if (hasFeaturePDU) {
      connection->OnReceiveFeatureSet(H460_MessageType::e_setup, fs);
  }
}

template <typename PDUType>
static void ReceiveFeatureData(const H323Connection * connection, unsigned code, const PDUType & pdu)
{
  if (pdu.m_h323_uu_pdu.HasOptionalField(H225_H323_UU_PDU::e_genericData)) {
    H225_FeatureSet fs;
    fs.IncludeOptionalField(H225_FeatureSet::e_supportedFeatures);
    H225_ArrayOf_FeatureDescriptor & fsn = fs.m_supportedFeatures;
    const H225_ArrayOf_GenericData & data = pdu.m_h323_uu_pdu.m_genericData;
    for (PINDEX i=0; i < data.GetSize(); i++) {
      PINDEX lastPos = fsn.GetSize();
      fsn.SetSize(lastPos+1);
      fsn[lastPos] = (H225_FeatureDescriptor &)data[i];
    }
    connection->OnReceiveFeatureSet(code, fs);
  }
}


template <typename PDUType>
static void ReceiveFeatureSet(const H323Connection * connection, unsigned code, const PDUType & pdu)
{
    if (pdu.HasOptionalField(PDUType::e_featureSet))
      connection->OnReceiveFeatureSet(code, pdu.m_featureSet);
}
#endif // OPAL_H460

//
//
//
//
//
// BuildFastStartList 
//
//
static PBoolean BuildFastStartList(const H323Channel & channel,
                               H225_ArrayOf_PASN_OctetString & array,
                               H323Channel::Directions reverseDirection) {
  H245_OpenLogicalChannel open;
  const H323Capability & capability = channel.GetCapability();

  PTRACE(5, "S3\tBuildFastStartList decoded capability: " << capability);

  if (channel.GetDirection() != reverseDirection) {
      if (!capability.OnSendingPDU(open.m_forwardLogicalChannelParameters.m_dataType)) {
      PTRACE(5, "S3\tcapability.OnSendingPDU returned false");
      return PFalse;
    }
  }
    else {
    if (!capability.OnSendingPDU(open.m_reverseLogicalChannelParameters.m_dataType)) {
      PTRACE(5, "S3\tcapability.OnSendingPDU returned false");
      return PFalse;
    } 
      open.m_forwardLogicalChannelParameters.m_multiplexParameters.SetTag(
                H245_OpenLogicalChannel_forwardLogicalChannelParameters_multiplexParameters::e_none);
      open.m_forwardLogicalChannelParameters.m_dataType.SetTag(H245_DataType::e_nullData);
      open.IncludeOptionalField(H245_OpenLogicalChannel::e_reverseLogicalChannelParameters);
    }

  if (!channel.OnSendingPDU(open)) {
    PTRACE(5, "S3\tchannel.OnSendingPDU returned false");
    return PFalse;
  }

  PTRACE(4, "H225\tBuild fastStart:\n  " << setprecision(2) << open);
  PINDEX last = array.GetSize();
  array.SetSize(last+1);
  array[last].EncodeSubType(open);

  PTRACE(3, "H225\tBuilt fastStart for " << capability);
  return PTrue;
}

#endif // S3_OPAL_H323_CXX_CONNECTION_H
