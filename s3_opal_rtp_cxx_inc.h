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
* @file s3_opal_rtp_cxx_inc.h
* @brief Static non-member code borrowed from OPAL 
* @author Anton Bondarenko
* @version 1
* @date 2012-08-26
*/

#ifndef S3_OPAL_RTP_CXX_INC_H
#define S3_OPAL_RTP_CXX_INC_H

static const char * const PayloadTypesNames[RTP_DataFrame::LastKnownPayloadType] = {
  "PCMU",
  "FS1016",
  "G721",
  "GSM",
  "G723",
  "DVI4_8k",
  "DVI4_16k",
  "LPC",
  "PCMA",
  "G722",
  "L16_Stereo",
  "L16_Mono",
  "G723",
  "CN",
  "MPA",
  "G728",
  "DVI4_11k",
  "DVI4_22k",
  "G729",
  "CiscoCN",
  NULL, NULL, NULL, NULL, NULL,
  "CelB",
  "JPEG",
  NULL, NULL, NULL, NULL,
  "H261",
  "MPV",
  "MP2T",
  "H263",
  NULL, NULL, NULL,
  "T38"
};

#endif // S3_OPAL_RTP_CXX_INC_H