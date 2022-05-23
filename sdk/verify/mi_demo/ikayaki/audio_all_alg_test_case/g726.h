/*
	Copyright (c) 2013-2016 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
*/

#ifndef __G726_H_
#define __G726_H_


typedef struct stBitStreamState_s
{
    unsigned int u32BitStream;
    int s32Residue;
}stBitStreamState_t;

typedef struct stG726State_s stG726State_t;
typedef short (*G726DecoderFunc_t)(stG726State_t *s, unsigned char u8Code);
typedef unsigned char (*G726EncoderFunc_t)(stG726State_t *s, short s16Amp);

struct stG726State_s
{
    int s32Rate;
    int s32BitsPerSample;

    /*! I don't the mean of 'y', so change y to Y. */
    int s32YLocked;
    short s16YUnlocked;
    /*! I don't the mean of 'dm', so change dm to DM. */
    short s16DMShort;
    short s16DMLong;
    /*! I don't the mean of 's16AP', so change s16AP to s16AP. */
    short s16AP;
    short s16PolePortion[2];
    short s16ZeroPortion[6];
    short s16PreviousSigns[2];
    short s16DifferenceQuantized[6];
    short s16SignalRepresented[2];
    int s32ToneDetect;
    stBitStreamState_t stBitStream;
    G726EncoderFunc_t encodeFunc;
    G726DecoderFunc_t decodeFunc;
};

stG726State_t *g726_init(stG726State_t *s, int bit_rate);

int g726_decode(stG726State_t *s, short amp[], const unsigned char g726_data[], int g726_bytes);

int g726_encode(stG726State_t *s, unsigned char g726_data[], const short amp[], int len);

#endif //__G726_H_
