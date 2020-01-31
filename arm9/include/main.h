/*

			Copyright (C) 2017  Coto
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301
USA

*/

#ifndef __main9_h__
#define __main9_h__

#include "typedefsTGDS.h"
#include "dsregs.h"
#include "limitsTGDS.h"
#include "dldi.h"
#include "defs.h"

typedef struct sNDSHeader {
  char gameTitle[12];
  char gameCode[4];
  char makercode[2];
  uint8 unitCode;
  uint8 deviceType;           // type of device in the game card
  uint8 deviceSize;           // device capacity (1<<n Mbit)
  uint8 reserved1[9];
  uint8 romversion;
  uint8 flags;                // auto-boot flag

  uint32 arm9romSource;
  uint32 arm9executeAddress;
  uint32 arm9destination;
  uint32 arm9binarySize;

  uint32 arm7romSource;
  uint32 arm7executeAddress;
  uint32 arm7destination;
  uint32 arm7binarySize;

  uint32 filenameSource;
  uint32 filenameSize;
  uint32 fatSource;
  uint32 fatSize;

  uint32 arm9overlaySource;
  uint32 arm9overlaySize;
  uint32 arm7overlaySource;
  uint32 arm7overlaySize;

  uint32 cardControl13;  // used in modes 1 and 3
  uint32 cardControlBF;  // used in mode 2
  uint32 bannerOffset;

  uint16 secureCRC16;

  uint16 readTimeout;

  uint32 unknownRAM1;
  uint32 unknownRAM2;

  uint32 bfPrime1;
  uint32 bfPrime2;
  uint32 romSize;

  uint32 headerSize;
  uint32 zeros88[14];
  uint8 gbaLogo[156];
  uint16 logoCRC16;
  uint16 headerCRC16;

  uint32 debugRomSource;
  uint32 debugRomSize;
  uint32 debugRomDestination;
  uint32 offset_0x16C;

  uint8 zero[0x90];
} __attribute__ ((__packed__)) tNDSHeader;

typedef struct sNDSBanner {
  uint16 version;
  uint16 crc;
  uint8 reserved[28];
  uint8 icon[512];
  uint16 palette[16];
  uint16 titles[6][128];
} __attribute__ ((__packed__)) tNDSBanner;

static inline void copy_mac( unsigned char *dest, const unsigned char *mac) {
	memcpy( dest, mac, 6);
}

#ifdef __cplusplus
extern "C" {
#endif

extern int main(int _argc, sint8 **_argv);
extern char curChosenBrowseFile[MAX_TGDSFILENAME_LENGTH+1];
extern bool fillNDSLoaderContext(char * filename);
extern bool GDBEnabled;
extern struct FileClassList * thisFileList;

//Juglak ARM9 specific
extern unsigned char ieee80211ack[];
extern unsigned int RIPCc[32];
extern unsigned int *RIPC;

// Wifi Stuff
extern unsigned char *tx_queue[64],*rx_queue[64];
extern unsigned int *rx_sizes,*tx_sizes;
extern unsigned char my_mac[6];
extern unsigned short bec_seq ;

extern bool vblank_happened;

extern unsigned int RIPCcmd(unsigned int cmd, unsigned int arg);
extern unsigned int PingARM7(unsigned int x);
extern void InitWifiMem();
extern void WriteWifi(unsigned short addr, unsigned short data);
extern unsigned short ReadWifi(unsigned short addr);
extern void Wifi_SetChannel9Juglak(unsigned char channel);
extern void Wifi_SetRetry(unsigned char retry);
extern void GetMAC();
extern void SendFrame(unsigned char *data, int len);
extern unsigned char * RXNextFrame(int *size);
extern void SendACK(unsigned char *mac);
extern unsigned short computeBeaconChecksum(unsigned short *data, int length);
extern void SendBeacon(struct ds_advert *ad, int seq, int connected_clients);
extern void InitRSA(struct nds_rsaframe *rsa);
extern unsigned short BuildNDSData(unsigned char *d, unsigned short seq, struct nds_rsaframe *rsa, tNDSHeader *ndsh, FILE * fileHandle);

extern int bnum;
extern struct ds_advert *ad;
extern void Timer_Beacons();

extern int CompareMAC(u8* a, u8* b);
extern char ts[128];

extern unsigned char rsapkt[];
extern unsigned char beacon[];
extern unsigned char authres[];
extern unsigned char assocres[];
extern unsigned char pingdata[];
extern unsigned char enddata[];
extern unsigned char ackdata[];

#ifdef __cplusplus
}
#endif

// make these accessable by both cores
#define rx_count RIPC[16]
#define tx_count RIPC[17]
#define rx_base RIPC[18]
#define tx_base RIPC[19]
#define WIFI_READY RIPC[20]

#define mymac my_mac


#endif
