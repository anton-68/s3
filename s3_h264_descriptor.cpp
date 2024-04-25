/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
/**
* @file s3_h264_descriptor.cpp
* @brief 
* @author Anton Bondarenko
* @version 1
* @date 2012-09-03
*/

#include "s3_h264_descriptor.h"
#include "s3_h323_h264_capability.h"




//                      GetOpalH264 

const OpalVideoFormat & GetOpalH264() 
{
    PTRACE(5, "S3\t->S3H264Descriptor::GetOpalH264");
    static const OpalVideoFormat H264_Format(OPAL_H264, RTP_DataFrame::DynamicBase, "H264", 4096, 2304, 172, 10240000);
    static H323CapabilityFactory::Worker<S3H323H264Capability> H264_Factory(OPAL_H264, true);
    return H264_Format;
}




//  ENCODING-DECODING DATA

unsigned S3H264Descriptor::hexMapArraySize = 22;

S3H264Descriptor::HexMap S3H264Descriptor::bc[] = {
            {0, HEX("0000"), '0'},
            {1, HEX("0001"), '1'},
            {2, HEX("0010"), '2'},
            {3, HEX("0011"), '3'},
            {4, HEX("0100"), '4'},
            {5, HEX("0101"), '5'},
            {6, HEX("0110"), '6'},
            {7, HEX("0111"), '7'},
            {8, HEX("1000"), '8'},
            {9, HEX("1001"), '9'},
            {10, HEX("1010"), 'a'},
            {10, HEX("1010"), 'A'},
            {11, HEX("1011"), 'b'},
            {11, HEX("1011"), 'B'},
            {12, HEX("1100"), 'c'},
            {12, HEX("1100"), 'C'},
            {13, HEX("1101"), 'd'},
            {13, HEX("1101"), 'D'},
            {13, HEX("1110"), 'e'},
            {13, HEX("1110"), 'E'},
            {13, HEX("1111"), 'f'},
            {13, HEX("1111"), 'F'}
};      

unsigned S3H264Descriptor::profileDecodeArraySize = 15;

S3H264Descriptor::ProfileDecode S3H264Descriptor::profileDecode[] = {
            {CB, -1, "42", BITMAP("01000000"), BITMAP("01001111")},
            {CB, -1, "4D", BITMAP("10000000"), BITMAP("10001111")},
            {CB, -1, "58", BITMAP("11000000"), BITMAP("11001111")},
            {B, 64, "42", BITMAP("00000000"), BITMAP("01001111")},
            {B, 64, "58", BITMAP("10000000"), BITMAP("11001111")},
            {M, 32, "4D", BITMAP("00000000"), BITMAP("10101111")},
            {E, 16, "58", BITMAP("00000000"), BITMAP("11111111")},
            {H, 8, "64", BITMAP("00000000"), BITMAP("11111111")},
            {H10, 4, "6E", BITMAP("00000000"), BITMAP("11111111")},
            {H42, 2, "7A", BITMAP("00000000"), BITMAP("11111111")},
            {H44, 1, "F4", BITMAP("00000000"), BITMAP("11111111")},
            {H10I, -1, "6E", BITMAP("00010000"), BITMAP("11111111")},
            {H42I, -1, "7A", BITMAP("00010000"), BITMAP("11111111")},
            {H44I, -1, "F4", BITMAP("00010000"), BITMAP("11111111")},
            {C44I, -1, "2C", BITMAP("00010000"), BITMAP("11111111")}
};

unsigned S3H264Descriptor::profileEncodeArraySize = 12;

S3H264Descriptor::ProfileEncode S3H264Descriptor::profileEncode[] = {
            {CB, "Constrained Baseline", 64, "42", BITMAP("01000000")},
            {B, "Baseline", 64, "42", BITMAP("00000000")},
            {M, "Main", 32, "4D", BITMAP("00000000")},
            {E, "Extended", 16, "58", BITMAP("00000000")},
            {H, "High", 8, "64", BITMAP("00000000")},
            {H10, "High 10", 4, "6E", BITMAP("00000000")},  
            {H42, "High 4:2:2", 2, "7A", BITMAP("00000000")},
            {H44, "High 4:4:4", 1, "F4", BITMAP("00000000")},
            {H10I, "High 10 Intra", 0, "6E", BITMAP("00010000")},
            {H42I, "High 4:2:2 Intra", 0, "7A", BITMAP("00010000")},
            {H44I, "High 4:4:4 Intra", 0, "F4", BITMAP("00010000")},
            {C44I, "CAVLC 4:4:4 Intra", 0, "2C", BITMAP("00010000")}
};

unsigned S3H264Descriptor::levelDecodeArraySize = 17;

S3H264Descriptor::LevelDecode S3H264Descriptor::levelDecode[] = {
            {L1, 15, "0A", BITMAP("00010000"), BITMAP("00000000")},
            {L1_b, 19, "0B", BITMAP("00010000"), BITMAP("00010000")},
            {L1_1, 22, "0B", BITMAP("00010000"), BITMAP("00000000")},
            {L1_2, 29, "0C", BITMAP("00010000"), BITMAP("00000000")},
            {L1_3, 36, "0D", BITMAP("00010000"), BITMAP("00000000")},
            {L2, 43, "14", BITMAP("00010000"), BITMAP("00000000")},
            {L2_1, 50, "15", BITMAP("00010000"), BITMAP("00000000")},
            {L2_2, 57, "16", BITMAP("00010000"), BITMAP("00000000")},
            {L3, 64, "1E", BITMAP("00010000"), BITMAP("00000000")},
            {L3_1, 71, "1F", BITMAP("00010000"), BITMAP("00000000")},
            {L3_2, 78, "20", BITMAP("00010000"), BITMAP("00000000")},
            {L4, 85, "28", BITMAP("00010000"), BITMAP("00000000")},
            {L4_1, 92, "29", BITMAP("00010000"), BITMAP("00000000")},
            {L4_2, 99, "2A", BITMAP("00010000"), BITMAP("00000000")},
            {L5, 106, "32", BITMAP("00010000"), BITMAP("00000000")},
            {L5_1, 113, "33", BITMAP("00010000"), BITMAP("00000000")},
            {L5_2, 120, "34", BITMAP("00010000"), BITMAP("00000000")}
};

unsigned S3H264Descriptor::levelEncodeArraySize = 17;

S3H264Descriptor::LevelEncode S3H264Descriptor::levelEncode[] = {
            {L1, "1", 15, "0A", BITMAP("00000000")},
            {L1_b, "1b",  19, "0B", BITMAP("00010000")},
            {L1_1, "1.1", 22, "0B", BITMAP("00000000")},
            {L1_2, "1.2", 29, "0C", BITMAP("00000000")},
            {L1_3, "1.3", 36, "0D", BITMAP("00000000")},
            {L2, "2", 43, "14", BITMAP("00000000")},
            {L2_1, "2.1", 50, "15", BITMAP("00000000")},
            {L2_2, "2.2", 57, "16", BITMAP("00000000")},
            {L3, "3", 64, "1E", BITMAP("00000000")},
            {L3_1, "3.1", 71, "1F", BITMAP("00000000")},
            {L3_2, "3.2", 78, "20", BITMAP("00000000")},
            {L4, "4", 85, "28", BITMAP("00000000")},
            {L4_1, "4.1", 92, "29", BITMAP("00000000")},
            {L4_2, "4.2", 99, "2A", BITMAP("00000000")},
            {L5, "5", 106, "32", BITMAP("00000000")},
            {L5_1, "5.1", 113, "33", BITMAP("00000000")},
            {L5_2, "5.2", 120, "34", BITMAP("00000000")}
};  

//  ENCODING-DECODING DATA END




//                               +

S3H264Descriptor::Bitmap operator+(const S3H264Descriptor::Hex & b0, const S3H264Descriptor::Hex & b1)
{
    S3H264Descriptor::Bitmap result;
    for(unsigned i = 0; i < 4; i++) 
    {
        result[i] = b1[i];
        result[i+4] = b0[i];
    }
    return result;
}




//                S3H264Descriptor

S3H264Descriptor::S3H264Descriptor(const H245_DataType & dataType,
              const H245_H2250LogicalChannelParameters & parameters,
                                          const unsigned direction,
                                          const unsigned role)
                    : S3MediaDescriptor(dataType, parameters, direction, role) 
{
    PTRACE(5, "S3\t::SH264Descriptor");

    h264Profile = B;
    capabilityTag = h264;
    typeName = "H264";
                                      
    // Extract packetization
    const H245_RTPPayloadType_payloadDescriptor * payloadDescriptor = NULL;
    if(parameters.HasOptionalField(H245_H2250LogicalChannelParameters::e_mediaPacketization)) 
    {    
        payloadDescriptor = &((const H245_RTPPayloadType &)parameters.m_mediaPacketization).m_payloadDescriptor;
        PASN_ObjectId oid = ((const PASN_ObjectId &)(*payloadDescriptor));
        payloadNumber = ((const H245_RTPPayloadType &)parameters.m_mediaPacketization).m_payloadType.GetValue();

        h264PacketizationMode = static_cast<PacketizationMode>(oid[7]);
    } 
    else 
    { 
        PTRACE(2, "S3\tEmpty packetization mode for H.264 call defaulted to single NAL");
        h264PacketizationMode = singleNAL;
    }

    PASN_Integer tmp;
    H245_ArrayOf_GenericParameter pid = ((const H245_GenericCapability &)((const H245_VideoCapability &)dataType)).m_collapsing;
    tmp = pid[0].m_parameterValue;
    h264Profile = getProfile(tmp.GetValue());
    tmp = pid[1].m_parameterValue;
    h264Level[role] = getLevel(tmp.GetValue());
    if(role == originator)
        h264Level[terminator] = UndefinedLevel;
    else
        h264Level[originator] = UndefinedLevel;
}




//                 S3H264Descriptor

S3H264Descriptor::S3H264Descriptor(const OpalMediaType & type,
                                  const SDPMediaFormat & format,
                            const OpalTransportAddress & address,
                                            const WORD & port,
                  const SDPMediaDescription::Direction & direction,
                         const S3MediaDescriptor::Role & role)
                            : S3MediaDescriptor(type, format, address, port, direction, role) 
{
        PTRACE(5, "S3\t::SH264Descriptor");

        h264Profile = B;
        //h264Level[originator] = L1_1; 
        //h264Level[terminator] = L1_1; 
        capabilityTag = h264;
        typeName = "H264";
        h264PacketizationMode = singleNAL;

        PStringArray fmtp = static_cast<const S3SDPMediaFormat*>(&format)->getFMTPString().Tokenise(" ;");
        PStringArray tokens;
        for(PINDEX i = 0; i < fmtp[i].GetSize(); i++)
        {
            tokens = fmtp[i].Tokenise("=");
            if(tokens[0] == "profile-level-id") 
                decodeProfileLevelId(tokens[1], h264Profile, h264Level[role]);
            else
                if (tokens[0] == "packetization-mode") 
                    switch (tokens[1][0]) 
                    {
                        case '1' :
                            h264PacketizationMode = non_interleaved;
                            break;
                        case '2' :
                            h264PacketizationMode = interleaved;
                            break;
                        default :
                            h264PacketizationMode = singleNAL;
                            break;
                    }
        }

        if(role == originator)
            h264Level[terminator] = UndefinedLevel;
        else
            h264Level[originator] = UndefinedLevel;
}




//                                          =

S3H264Descriptor& S3H264Descriptor::operator=(const S3H264Descriptor & rhs) 
{
    this->S3MediaDescriptor::operator=(rhs);
    this->h264Profile = rhs.h264Profile;
    this->h264Level[originator] = rhs.h264Level[originator];
    this->h264Level[terminator] = rhs.h264Level[terminator];
    this->h264PacketizationMode = rhs.h264PacketizationMode;
    return *this;
}




//                         mergeH323OLC

PBoolean S3H264Descriptor::mergeH323OLC(S3H264Descriptor &descriptor)
{
    PTRACE(5, "S3\t->S3H264Descriptor::mergeH323OLC");

    if(*this != descriptor)
    {
        PTRACE(2, "S3\tS3H264Descriptor::mergeH323OLC error - incompatible descriptors");
        return false;   
    }

    S3MediaDescriptor::Role role;
    for(unsigned i = 0; i < 2; i++) 
    {
        role = static_cast<S3MediaDescriptor::Role>(i);
        if(descriptor.getLevel(role) != UndefinedLevel) 
        {
            if(h264Level[role] == UndefinedLevel)
                h264Level[role] = descriptor.getLevel(role);
            else 
            {
                PTRACE(2, "S3\tS3H264Descriptor::mergeH323OLC error - descriptors are not complementary");
                return false;
            }
        }
    }

    return true;
}




//                         merge

PBoolean S3H264Descriptor::merge(S3H264Descriptor &descriptor)
{
    PTRACE(5, "S3\t->S3H264Descriptor::merge");

    if(mediaPort[originator] * mediaPort[terminator] == 0)
        static_cast<S3MediaDescriptor*>(this)->merge(*(static_cast<S3MediaDescriptor*>(&descriptor)));

    if(mergeH323OLC(descriptor))
    {
        if(h264Level[originator] == UndefinedLevel)
            h264Level[originator] = descriptor.getLevel(originator);
        if(h264Level[terminator] == UndefinedLevel)
            h264Level[terminator] = descriptor.getLevel(terminator);

        return true;
    }
    else 
    {
        PTRACE(2, "S3\tS3H264Descriptor::mergeH323OLC failed");
        return false;       
    }
}




//               <<

ostream &operator<<(ostream & os, const S3H264Descriptor & descriptor) 
{
    return descriptor.s3print(os);
}




//                          s3print

ostream & S3H264Descriptor::s3print(ostream & os) const 
{
    S3MediaDescriptor::s3print(os);
        os << "H264 packetization mode: " << h264PacketizationMode 
           << ", H264 profile: " << h264Profile 
           << ", H264 originator level: " << h264Level[originator]
           << ", H264 terminator level: " << h264Level[terminator] << endl;
    return os;
}




//               ==

PBoolean operator==(const S3H264Descriptor & lEx, const S3H264Descriptor & rEx) 
{
    return lEx.s3compare(rEx);
}




//               !=

PBoolean operator!=(const S3H264Descriptor & lEx, const S3H264Descriptor & rEx) 
{ 
    return !lEx.s3compare(rEx);
}




//                         s3compare

PBoolean S3H264Descriptor::s3compare(const S3H264Descriptor & rEx) const         //level is not checked!
{
    PTRACE(5, "S3\t->S3H264Descriptor::s3compare");
    return static_cast<const S3MediaDescriptor* const>(this)->s3compare(rEx) &&  //codec tag & type of media
           h264PacketizationMode == rEx.h264PacketizationMode &&                 //packetization mode
           h264Profile == rEx.h264Profile;                                       //profile
}




//                                   getSDPFormat

S3SDPMediaFormat * S3H264Descriptor::getSDPFormat(SDPMediaDescription &sdpDescriptor) const 
{
    PTRACE(5, "S3\t->S3H264Descriptor::getSDPFormat");
    // Base format constructor
    S3SDPMediaFormat * sdpFormat = S3MediaDescriptor::getSDPFormat(sdpDescriptor);
    // Encoding name    
    sdpFormat->SetEncodingName("H264"); //to refactor
    // clockrate
    sdpFormat->SetClockRate(90000); // to refactor
    // profile-level-id
    S3H264Descriptor::Level level;
    if(h264Level[terminator] == UndefinedLevel)
        level = h264Level[originator];
    else
        level = h264Level[h264Level[originator]<=h264Level[terminator]?originator:terminator]; 
    PString fmtp = "profile-level-id=";
    fmtp += *getProfileLevelId(h264Profile, level); 
    //packetization
    switch (h264PacketizationMode) 
    {
        case non_interleaved :
            fmtp += "; packetization-mode=1";
            break;
        case interleaved :
            fmtp += "; packetization-mode=2";
            break;
        default :
            break;
    }
    PTRACE(5, "S3\tCheck fmtp for h264: " << fmtp);
    sdpFormat->SetFMTP(fmtp);
    return sdpFormat;
}




//                     decodeProfileLevelId

void S3H264Descriptor::decodeProfileLevelId(const PString & pliString, Profile & profile, Level & level)
{
    PTRACE(5, "S3\t->S3H264Descriptor::decodeProfileLevel");
    PString profileStr = pliString(0,1);
    PString levelStr = pliString(4,5);
    Bitmap iopBitmap = Bitmap(asHex(pliString[2]) + asHex(pliString[3]));
    profile = getProfile(profileStr, iopBitmap);
    level = getLevel(levelStr, iopBitmap);
}




//                        getProfile

S3H264Descriptor::Profile S3H264Descriptor::getProfile(PString & profileStr, Bitmap iop) const 
{
    PTRACE(5, "S3\t->S3H264Descriptor::getProfile");
    Bitmap tmp;
    for(unsigned i = 0; i < S3H264Descriptor::profileDecodeArraySize; i++) 
    {
        tmp = (profileDecode[i].profileIOP ^= iop) &= profileDecode[i].profileIOPMask; 
        if(profileDecode[i].profileIDC == profileStr && tmp.to_ulong() == 0) 
            return profileDecode[i].profile;
    }
    PTRACE(2, "S3\tProfile not found");
    return S3H264Descriptor::UndefinedProfile;
}




//                                          getProfile

S3H264Descriptor::Profile S3H264Descriptor::getProfile(unsigned h323Profile) const 
{
    PTRACE(5, "S3\t->S3H264Descriptor::getProfile");
    for(unsigned i = 0; i < S3H264Descriptor::profileDecodeArraySize; i++)
        if(profileDecode[i].h323Integer == h323Profile)
            return profileDecode[i].profile;
    PTRACE(2, "S3\tProfile not found");
    return S3H264Descriptor::UndefinedProfile;
}




//                         getH264Profile

unsigned S3H264Descriptor::getH264Profile() const 
{
    PTRACE(5, "S3\t->S3H264Descriptor::getProfile");
    for(unsigned i = 0; i < S3H264Descriptor::profileDecodeArraySize; i++)
        if(profileDecode[i].profile  == h264Profile)
            return profileDecode[i].h323Integer;
    PTRACE(2, "S3\tProfile not found");
    return B;  // default
}





//                        getLevel

S3H264Descriptor::Level S3H264Descriptor::getLevel(PString & levelStr, Bitmap iop) const 
{
    PTRACE(5, "S3\t->S3H264Descriptor::getLevel");
    Bitmap tmp;
    for(unsigned i = 0; i < S3H264Descriptor::levelDecodeArraySize; i++) 
    {
        tmp = (levelDecode[i].levelIOP ^= iop) &= levelDecode[i].levelIOPMask; 
        if(levelDecode[i].levelIDC == levelStr && tmp.to_ulong() == 0) 
            return levelDecode[i].level;
    }
    PTRACE(2, "S3\tLevel not found");
    return S3H264Descriptor::UndefinedLevel;
}




//                                        getLevel

S3H264Descriptor::Level S3H264Descriptor::getLevel(unsigned h323Level) const 
{
    PTRACE(5, "S3\t->S3H264Descriptor::getLevel");
    for(unsigned i = 0; i < S3H264Descriptor::levelDecodeArraySize; i++)
        if(levelDecode[i].h323Integer == h323Level)
            return levelDecode[i].level;
    PTRACE(2, "S3\tLevel not found");
    return S3H264Descriptor::UndefinedLevel;
}




//                         getH264Level

unsigned S3H264Descriptor::getH264Level() const 
{
    PTRACE(5, "S3\t->S3H264Descriptor::getH264Level");
    S3H264Descriptor::Level level;
    if(h264Level[terminator] == UndefinedLevel)
        level = h264Level[originator];
    else
        level = h264Level[h264Level[originator]<=h264Level[terminator]?originator:terminator];
    for(unsigned i = 0; i < S3H264Descriptor::levelDecodeArraySize; i++)
        if(levelDecode[i].level == level)
            return levelDecode[i].h323Integer;
    PTRACE(2, "S3\tLevel not found");
    return L1_1; // default
}




//                       asStr

char & S3H264Descriptor::asStr(const Hex & h)
{
    PTRACE(5, "S3\t->S3H264Descriptor::asStr");
    PString result;
    unsigned i = 0; 
    while(bc[i].binary != h) i++;
    return bc[i].symbol;
}




//                          asStr

PString * S3H264Descriptor::asStr(const Bitmap & b)
{
    PTRACE(5, "S3\t->S3H264Descriptor::asStr");
    PString * result = new PString;
    Hex tmp;
    for(unsigned i = 0; i < 4; i++) tmp[i] = b[i+4];
    *result += asStr(tmp);
    for(unsigned i = 0; i < 4; i++) tmp[i] = b[i];
    *result += asStr(tmp);
    return result;
}




//                          asHex

S3H264Descriptor::Hex S3H264Descriptor::asHex(const char & c)
{
    PTRACE(5, "S3\t->S3H264Descriptor::asStr");
    unsigned i = 0; 
    while(i < hexMapArraySize && bc[i].symbol != c) i++;
    if(i == hexMapArraySize) 
    {
        PTRACE(1, "S3\tWrong string representation of hex number");
    }
    return bc[i].binary;
}




//                          getProfileLevelId

PString * S3H264Descriptor::getProfileLevelId(const Profile profile, const Level level)
{
    PTRACE(5, "S3\t->S3H264Descriptor::getProfileLevelId");
    unsigned idx = 0;
    while(idx < S3H264Descriptor::profileEncodeArraySize && profileEncode[idx].profile != profile) idx++; 
    PString * pli = new PString();
    *pli += profileEncode[idx].profileIDC;
    Bitmap iop = profileEncode[idx].profileIOP;
    idx = 0;
    while(idx < S3H264Descriptor::levelEncodeArraySize && levelEncode[idx].level != level) idx ++;
    iop = iop | levelEncode[idx].levelIOPinc;
    *pli += *asStr(iop);
    *pli += levelEncode[idx].levelIDC;
    return pli; 
}




//                                 getOpalCapability

H323Capability * S3H264Descriptor::getOpalCapability() const 
{
    PTRACE(5, "S3\t->S3H264Descriptor::getOpalCapability");
    H323Capability * capability = NULL;
    OpalMediaFormat format;
    if(isVideo() && capabilityTag == h264) 
    {
        capability = new S3H323H264Capability(this);
        format = GetOpalH264();
        capability->UpdateMediaFormat(format);
        return capability;
    }

    PTRACE(5, "S3\tS3H264Descriptor::getOpalCapability failed (wrong type)");
    return NULL;
}




//                         isComplete

PBoolean S3H264Descriptor::isComplete()
{
    PTRACE(5, "S3\t->S3H264Descriptor::isComplete");
    if(!S3MediaDescriptor::isComplete())
        return false;
    if( (h264Profile == UndefinedProfile) /*||
        (h264Level[originator] == UndefinedLevel) ||
        (h264Level[terminator] == UndefinedLevel)*/ ) 
    {
            PTRACE(1, "S3\tS3H264Descriptor incomplete :");
            s3print(cout);
            return false;
    }
    return true;
}
