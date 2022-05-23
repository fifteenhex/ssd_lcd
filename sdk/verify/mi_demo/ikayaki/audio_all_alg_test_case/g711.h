/*
	Copyright (c) 2013-2016 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
*/
#ifndef __G711_H_
#define __G711_H_


void G711Encoder(short *pcm,unsigned char *code,int size,int lawflag);

void G711Decoder(short *pcm,unsigned char *code,int size,int lawflag);

void G711Covert(unsigned char *dst, unsigned char *src, int size, int flag);

#endif //__G711_H_
