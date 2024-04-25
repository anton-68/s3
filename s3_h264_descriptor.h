/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
/**
* @file s3_h264_descriptor.h
* @brief 
* @author Anton Bondarenko
* @version 1
* @date 2012-09-03
*/

#ifndef     S3_H264_DESCRIPTOR_H
#define     S3_H264_DESCRIPTOR_H

#define     BITMAP( bits ) ( Bitmap ( string( bits ) ) )
#define     HEX( bits ) ( Hex ( string( bits ) ) )

#include <bitset>
#include "s3_media_descriptor.h"

class S3H264Descriptor : public S3MediaDescriptor 
{
    PCLASSINFO(S3H264Descriptor, S3MediaDescriptor);

    friend ostream &operator<<(ostream &, const S3H264Descriptor &);

    friend PBoolean operator==(const S3H264Descriptor & lEx, const S3H264Descriptor &);

    friend PBoolean operator!=(const S3H264Descriptor & lEx, const S3H264Descriptor &);

    typedef bitset<8> Bitmap;

    typedef bitset<4> Hex;

    friend Bitmap operator+(const Hex &, const Hex &);

    public:

        struct HexMap 
        {
            unsigned value;
            Hex binary;
            char symbol;
        }; 

        enum Profile 
        {
            UndefinedProfile = 0,
            CB = 128,
            B = 64,
            M = 32,
            E = 16,
            H = 8,
            H10 = 4,
            H42 = 2,
            H44 = 1,
            H10I = 254,
            H42I = 253,
            H44I = 251,
            C44I = 247
        };

        struct ProfileDecode 
        {
            Profile profile;
            unsigned h323Integer;
            PString profileIDC;
            Bitmap profileIOPMask;
            Bitmap profileIOP;
        };

        struct ProfileEncode 
        {
            Profile profile;
            PString name;
            unsigned h323Integer;
            PString profileIDC;
            Bitmap profileIOP;
        };

        enum Level 
        {
            UndefinedLevel = 0,
            L1 = 15,
            L1_b = 19,
            L1_1 = 22,
            L1_2 = 29,
            L1_3 = 36,
            L2 = 43,
            L2_1 = 50,
            L2_2 = 57,
            L3 = 64,
            L3_1 = 71,
            L3_2 = 78,
            L4 = 85,
            L4_1 = 92,
            L4_2 = 99,
            L5 = 106,
            L5_1 = 113,
            L5_2 = 120
        };

        struct LevelDecode 
        {
            Level level;
            unsigned h323Integer;
            PString levelIDC;
            Bitmap levelIOPMask;
            Bitmap levelIOP;
        }; 

        struct LevelEncode 
        {
            Level level;
            PString name;
            unsigned h323Integer;
            PString levelIDC;
            Bitmap levelIOPinc;
        };

        enum PacketizationMode 
        {
            singleNAL,
            non_interleaved,
            interleaved
        };

        S3H264Descriptor() : S3MediaDescriptor() {}

        S3H264Descriptor(const H245_DataType &,
                         const H245_H2250LogicalChannelParameters &,
                         const unsigned,
                         const unsigned);

        S3H264Descriptor(const OpalMediaType &,
                         const SDPMediaFormat &,
                         const OpalTransportAddress &,
                         const WORD &,
                         const SDPMediaDescription::Direction &,
                         const S3MediaDescriptor::Role &);
        
        S3H264Descriptor(const S3H264Descriptor & rhe) : S3MediaDescriptor(rhe) {*this = rhe;}

        virtual unsigned getPayloadType() const {return payloadNumber;}

        static char & asStr(const Hex &);

        static PString * asStr(const Bitmap &);

        static Hex asHex(const char &);
        
        Profile getProfile(unsigned) const;

        unsigned getH264Profile() const;

        Profile getProfile(PString &, Bitmap) const;
        
        Level getLevel(unsigned) const;

        unsigned getH264Level() const;

        Level getLevel(Role & role) const {return h264Level[role];}

        Level getLevel(PString &, Bitmap) const;

        static PString * getProfileLevelId(const Profile, const Level); 

        void decodeProfileLevelId(const PString &, Profile &, Level &);

        virtual PBoolean mergeH323OLC(S3H264Descriptor &);

        virtual PBoolean merge(S3H264Descriptor &);

        virtual S3H264Descriptor& operator=(const S3H264Descriptor &);

        virtual ostream& s3print(ostream&) const;

        virtual PBoolean s3compare(const S3H264Descriptor&) const;

        virtual S3SDPMediaFormat * getSDPFormat(SDPMediaDescription &) const;
    
        virtual H323Capability * getOpalCapability() const;

        virtual PBoolean isComplete();

    protected:

        static HexMap bc[];

        static unsigned hexMapArraySize;

        static ProfileDecode profileDecode[];

        static unsigned profileDecodeArraySize;

        static ProfileEncode profileEncode[];

        static unsigned profileEncodeArraySize;

        static LevelDecode levelDecode[];

        static unsigned levelDecodeArraySize;

        static LevelEncode levelEncode[];

        static unsigned levelEncodeArraySize;

        Profile h264Profile;

        Level h264Level[2];

        PacketizationMode h264PacketizationMode;
};
#endif  //  S3_H264_DESCRIPTOR_H
