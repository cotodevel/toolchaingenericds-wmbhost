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

#include "main.h"
#include "dsregs.h"
#include "dsregs_asm.h"
#include "typedefsTGDS.h"
#include "gui_console_connector.h"
#include "dswnifi_lib.h"
#include "TGDSLogoLZSSCompressed.h"
#include "ipcfifoTGDSUser.h"
#include "fatfslayerTGDS.h"
#include "cartHeader.h"
#include "ff.h"
#include "dldi.h"
#include "xmem.h"
#include "dmaTGDS.h"
#include "timerTGDS.h"
#include "nds_cp15_misc.h"
#include "fileBrowse.h"
#include "biosTGDS.h"
#include "global_settings.h"
#include "file.h"
#include <stdio.h>

/*
	WMB Host - v0.1
	By Juglak

*/

#define XMEM_BS 256
#define WMB_CHANNEL 13

/* Juglak ARM9 Code start */

unsigned char rsapkt[] = {
0x28, 0x02, 0x00, 0x02, 0x03, 0x09, 0xBF, 0x00, 0x00, 0x00, 0x00, 0x16, 0x56, 0x52, 0xE5, 0x04,
0x00, 0x16, 0x56, 0x52, 0xE5, 0x04, 0x80, 0xEC, 

0x06, 0x01, 0x02, 0x00, 0x75, 0x11, 0x03, 
0x00, 0x48, 0x00, 0x02, 
0x00, 0x00, 0x38, 0x02, 
0x00, 0x00, 0x00, 0x00, 
0x00, 0xFE, 0x7F, 0x02, 
0x00, 0xFE, 0x7F, 0x02, 
0x60, 0x01, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 
0x00, 0x40, 0x00, 0x02, 
0x00, 0x40, 0x00, 0x02, 
0xB8, 0x65, 0x09, 0x00, 
0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x2C, 0x02, 
0x00, 0x00, 0x38, 0x02, 
0x24, 0x4B, 0x02, 0x00, 
0x01, 0x00, 0x00, 0x00, 

0x61, 0x63, 0x01, 0x00, 0x16, 0x5A, 0xD4, 0x4D, 0x02, 0x43, 0xDE, 0xB2, 0x28, 0x53, 0x09, 0xD2, 
0xE0, 0x82, 0x51, 0x0F, 0x77, 0x58, 0xF8, 0x08, 0xB2, 0x1A, 0xC8, 0x65, 0x6D, 0x75, 0xB1, 0xE0, 
0xB3, 0x38, 0x20, 0x59, 0x03, 0x4E, 0xE6, 0xB4, 0x3B, 0x45, 0x96, 0xBC, 0x49, 0x98, 0xCE, 0x13, 
0x29, 0x61, 0xCF, 0xE2, 0x0C, 0xC2, 0x18, 0x69, 0x67, 0x11, 0xF3, 0xF4, 0x96, 0x66, 0xA0, 0x0E, 
0x81, 0x8C, 0x9C, 0x8B, 0x76, 0x81, 0xAC, 0x37, 0xC8, 0x40, 0x1D, 0x5C, 0x9E, 0xBB, 0xE8, 0xFD, 
0x11, 0xFD, 0x46, 0xD9, 0x74, 0x10, 0xAA, 0x92, 0x47, 0xD0, 0x84, 0xE7, 0x45, 0xD9, 0x8D, 0x5E, 
0x25, 0xD6, 0xD0, 0x09, 0x3F, 0x08, 0xAD, 0x88, 0xB5, 0x6E, 0xD5, 0xD0, 0xD5, 0x2B, 0x17, 0x32, 
0xD0, 0xEF, 0x10, 0xF6, 0x67, 0x8F, 0x6A, 0x07, 0xC3, 0x33, 0x83, 0x58, 0x8A, 0xC2, 0x17, 0x29, 
0x93, 0xF9, 0xD0, 0xAA, 0x5D, 0x8D, 0x98, 0x01,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00,
0x00, 0x02, 0x00
};


unsigned char beacon[] = {
0x80, 0x00, 
0x00, 0x00, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // to MAC
0x00, 0x12, 0x17, 0x91, 0x92, 0x28, // my mac (from)
0x00, 0x12, 0x17, 0x91, 0x92, 0x28, // my mac (bss)
//22

0x00, 0x00, 0xE8, 0x37, 0x14, 0x20, 0x05, 0x00, 0x00, 0x00, 0xC8, 0x00, 0x21, 0x00, 
//36
// element
0x01, 0x02, 
	0x82, 0x84, 
// element
0x03, 0x01, 
	0x0D, 
// element
0x05, 0x05, 
	0x00, 0x02, 0x00, 0x00, 0x00, 
//50
// DS element
0xDD, 0x88, //52
	0x00, 0x09, 0xBF, 
	0x00, 0x0A, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xFF, 
	0x00, 0x0B, 0xFE, 0x01, 0x08, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0xDF, 0x44, 
	0x00, 
	0x09, 
	0x62, 0x00, 
	// data
	0x1F, 0x7C, 0xCE, 0x39, 0xCE, 0x45, 0x26, 0x16, 
	0xA9, 0x1A, 0x4A, 0x2A, 0x2B, 0x1F, 0x4E, 0x27, 
	0xE6, 0x15, 0xD0, 0x3E, 0xD3, 0x37, 0x72, 0x4E, 
	0x73, 0x66, 0x96, 0x4F, 0xD9, 0x57, 0xDD, 0x77, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0xFF, 0x0F, 0x00, 0xF0, 0xAF, 0xF9,
	0x00, 0xFF, 0x4A, 0xF5, 0x00, 0xEF, 0x57, 0xFF, 
	0xF0, 0xAF, 0xD3, 0xEF, 0xF0, 0x7E, 0xF8, 0xAF, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0xFF, 0x0F, 0x00, 0x00, 
	0xEF, 0x0F, 0xF0, 0xFF, 0x4A, 0x0D, 0xFF, 0x2B, 
	0x57, 0xFF, 0x2F, 0xB1, 0xD4, 0xFF, 0x2B, 0xFF, 
	0x00, 0x00
}; // 188 bytes


unsigned char authres[] = {
	0xB0, 0x00, 
	0xA2, 0x00, 
	//0,0,
	0x00, 0x16, 0x56, 0x52, 0xE5, 0x04, 0x00, 0x12, 0x17, 0x91, 0x92, 0x28,
	0x00, 0x12, 0x17, 0x91, 0x92, 0x28, 0xB0, 0x05, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00
};

unsigned char assocres[] = {
	0x10, 
	0x00, 0xA2, 
	//0,0,
	0x00, 0x00, 0x16, 0x56, 0x52, 0xE5, 0x04, 0x00, 0x12, 0x17, 0x91, 0x92, 0x28,
	0x00, 0x12, 0x17, 0x91, 0x92, 0x28, 0xD0, 0x05, 0x21, 0x00, 0x00, 0x00, 0x01, 0xC0, 0x01, 0x02,
	0x82, 0x84
};

unsigned char pingdata[] = {
	0x28, 0x02, 
	0x00, 0x02, 
	//0,0,
	0x03, 0x09, 0xBF, 0x00, 0x00, 0x00, 
	0x00, 0x12, 0x17, 0x91, 0x92, 0x28,
	0x00, 0x12, 0x17, 0x91, 0x92, 0x28, 
	
	0xF0, 0x05, 
	
	0x06, 0x01, 0x02, 0x00, 0x03, 0x11, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x02, 0x00
};

unsigned char enddata[] = {
	0x28, 0x02, 
	0x00, 0x02, 
	0x03, 0x09, 0xBF, 0x00, 0x00, 0x00, 
	0x00, 0x12, 0x17, 0x91, 0x92, 0x28,
	0x00, 0x12, 0x17, 0x91, 0x92, 0x28, 
	0x00, 0x72, 
	
	0x06, 0x01, 0x02, 0x00, 0x03, 0x11, 0x05, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x02, 0x00
};


unsigned char ackdata[] = {
	0x18, 0x02, 0x00, 0x00, 0x03, 0x09, 0xBF, 0x00, 0x00, 0x03, 0x00, 0x12, 0x17, 0x91, 0x92, 0x28,
	0x00, 0x12, 0x17, 0x91, 0x92, 0x28, 0x20, 0x06, 0x3B, 0x00, 0x00, 0x00
};


unsigned char ieee80211ack[] = {
	0xD4, 0, 0, 0, 
	00, 0x12, 0x17, 0x91, 0x92, 0x28 // mac to ack
};

int bnum=0;
struct ds_advert *ad = NULL;
char ts[128];

unsigned int RIPCc[32];
unsigned int *RIPC;

// Wifi Stuff
unsigned char *tx_queue[64],*rx_queue[64];
unsigned int *rx_sizes,*tx_sizes;
unsigned char my_mac[6];
unsigned short bec_seq = 0;

bool vblank_happened = false;

unsigned int RIPCcmd(unsigned int cmd, unsigned int arg){
	// simple RIPC cmd
	// wait for cmd to be clear
	RIPC[1] = arg;
	RIPC[0] = cmd;
	while(RIPC[0] != 0) swiDelay(1);
	return RIPC[2];
}

unsigned int PingARM7(unsigned int x){
	x = RIPCcmd(1,x);
	return x;
}
	
void InitWifiMem(){
	// Allocates memory for wifi queues...
	unsigned int i;
	
	RIPC[1] = (unsigned int) Xcalloc(((64+64)*2048)+((64+64)*4),1);
	RIPC[1] |= 0x00400000; // disable caching on the wifi queue mem...

	for(i=0;i<64;i++) { rx_queue[i] = (unsigned char *) (unsigned int) ( RIPC[1] + (i*2048) ); }

	// setup TX queue mem...
	for(i=0;i<64;i++) { tx_queue[i] = (unsigned char *) (unsigned int) ( RIPC[1] + (i*2048) + 131072 ); }
			
	// setup RX queue sizes mem...
	rx_sizes = (unsigned int *) (unsigned int) ( RIPC[1] + 262144 );

	// setup RX queue sizes mem...
	tx_sizes = (unsigned int *) (unsigned int) ( RIPC[1] + 262144 + 256 );

	// tell arm7 the location of wifi queue mem...
	RIPC[0] = 2;
	while(RIPC[0] != 0) swiDelay(1);
}

void WriteWifi(unsigned short addr, unsigned short data){
	// set 16-bit wifi reg on arm7
	RIPC[1] = addr;
	RIPC[3] = data;
	RIPC[0] = 3;
	while(RIPC[0] != 0) swiDelay(1);
}	

unsigned short ReadWifi(unsigned short addr){
	// set 16-bit wifi reg on arm7
	RIPC[1] = addr;
	RIPC[0] = 4;
	while(RIPC[0] != 0) swiDelay(1);
	return RIPC[2] & 0xFFFF;
}

void Wifi_SetChannel9Juglak(unsigned char channel){
	if ((channel < 1) || (channel > 13)) return;
	RIPC[1] = channel;
	RIPC[0] = 6;
	while(RIPC[0] != 0) swiDelay(1);
}

void Wifi_SetRetry(unsigned char retry){
	RIPC[1] = retry;
	RIPC[0] = 7;
	while(RIPC[0] != 0) swiDelay(1);
}	

void GetMAC(){
	RIPC[0] = 8;
	while(RIPC[0] != 0) swiDelay(1);
	
	my_mac[0] = RIPC[3] & 0xFF;
	my_mac[1] = RIPC[4] & 0xFF;
	my_mac[2] = RIPC[5] & 0xFF;
	my_mac[3] = RIPC[6] & 0xFF;
	my_mac[4] = RIPC[7] & 0xFF;
	my_mac[5] = RIPC[8] & 0xFF;
}

void SendFrame(unsigned char *data, int len){
	coherent_user_range_by_size((uint32)&tx_queue[0][0], sizeof(tx_queue));
	
	int i, cf;
	cf = tx_base;
	for(i=0;i<12;i++) {
		tx_queue[cf][i] = 0;
	}

	tx_queue[cf][8] = 0x14; // Trans rate... a=1 14=2mbit
	
	// Data Size
	tx_queue[cf][10]	= (len+4)&0xFF;
	tx_queue[cf][11]	= ((len+4)&0xFF00)>>8;

	for(i=0;i<len;i++) {
		tx_queue[cf][i+12] = data[i];
	}

	tx_sizes[cf] = len+16;
	tx_base++;
	tx_count++;

	if(tx_base == 64) tx_base = 0;
}

unsigned char * RXNextFrame(int *size){
	int i, j;
	unsigned int fs;
	unsigned char *data;
	coherent_user_range_by_size((uint32)&rx_count, sizeof(rx_count));
	coherent_user_range_by_size((uint32)&rx_base, sizeof(rx_base));
	coherent_user_range_by_size((uint32)&rx_queue[0][0], sizeof(rx_queue));
	
	j = rx_count;
	i = rx_base;
	if (j) {
		// pop a packet
		rx_count--;
		i -= j;
		if (i<0) i+=64;
		if(i < sizeof(rx_queue)){
			data = rx_queue[i] + 12;
			unsigned short * thisPtr = (unsigned short *)&rx_queue[i][8];
			if(thisPtr != NULL){
				fs = *(thisPtr);
				if (fs > 0) {	
					*size = fs;
					return data;
				}	
			}
		}
	}
	return NULL;
}

void SendACK(unsigned char *mac){
	copy_mac(&ieee80211ack[4], mac);
	SendFrame(&ieee80211ack[0], 10);
}

// got this from some wiki somewhere... dabu for that...
unsigned short computeBeaconChecksum(unsigned short *data, int length){
  unsigned int sum = 0;
  int j = 0;
  for (j = 0; j < length; j++) sum += *data++;
  sum = (-((sum >> 16) + (sum & 0xFFFF) + 1)) & 0xFFFF;
  return sum;
}

void SendBeacon(struct ds_advert *ad, int seq, int connected_clients){
	unsigned char *b,*d;
	struct beacon *bc;
	struct ds_element *ds;
	int i;
	
	b = (unsigned char *) Xmalloc(188);
	bc = (struct beacon *) (unsigned int) b;
	
	for(i=0;i<188;i++) {
		b[i] = beacon[i];
	}
	
	ds = (struct ds_element *) (unsigned int) &beacon[52];

	ds->sequence_number = seq;
	ds->advert_sequence_number = seq;
	
	b[42] = 13; // channel we're broadcasting on
	if (seq == 9) {
		ds->non_advert = 2;
		ds->data_size = 1;
		ds->advert_sequence_number = 1; // max players at this point?
	}
	else {
		if (seq == 8) {
			ds->data_size = 72;
		}
		else {
			ds->data_size = 98;
		}
		
		ds->non_advert = 0;
	}

	ds->connected_clients = connected_clients;
	
	if (seq == 9) {
		for(i=0;i<98;i++) ds->data[i] = 0;
	}
	else {
		// copy data from ad into beacon
		d = (unsigned char *) (unsigned int) ad;
		d += 98*seq;
		if (seq == 8) {
			for(i=0;i<72;i++) ds->data[i] = d[i];
			for(i=72;i<98;i++) ds->data[i] = 0;
		}
		else {
			for(i=0;i<98;i++) ds->data[i] = d[i];
		}
	}
	
	ds->checksum = computeBeaconChecksum((unsigned short *) (unsigned int)(((unsigned int)&ds->data[0])-4), (ds->data_size+4)/2);

	// think we're done...
	bc->seq = bec_seq<<4;
	bec_seq++;
	
	copy_mac(bc->srcmac,mymac);
	copy_mac(bc->bssmac,mymac);
	
	SendFrame(b,188);
	Xfree(b);
	
}

bool InitRSA(struct nds_rsaframe *rsa, FILE * fileHandle){
	memset((u8*)rsa, 0, sizeof(struct nds_rsaframe));
	
	rsa->headerDestTemp = 0x027FFE00;
	rsa->headerDestActual = 0x027FFE00;
	rsa->headerSize = 0x160;
	rsa->arm7destTemp = 0x022C0000;
	rsa->unk38 = 1;
	
	//Append the RSAFrame if the NDS Binary has one (enables retail Demos to be detected on unmodified NintendoDS)
	int RSAOffset = getRSASignatureOffsetFromFileHandle(fileHandle);
	if(RSAOffset != -1){
		//Do RSA Fill
		int fLoc = ftell(fileHandle);
		fseek(fileHandle, RSAOffset, SEEK_SET);
		int read = fread((u8*)&rsa->signatureID, 1, sizeof(rsa->signatureID) + sizeof(rsa->signatureRSA), fileHandle);
		fseek(fileHandle, fLoc, SEEK_SET);
		//printf("Found RSAOffset! File Offset: %x", RSAOffset);
		return true;
	}
	else{
		//printf("RSAOffset not found!");
	}
	return false;
}

unsigned short BuildNDSData(unsigned char *d, unsigned short seq, struct nds_rsaframe *rsa, tNDSHeader *ndsh, FILE * fileHandle){	//u8* data
	unsigned char head[] = {
		0x28, 0x02, 0x00, 0x02, 0x03, 0x09, 0xBF, 0x00, 0x00, 0x00, 0x00, 0x12, 0x17, 0x91, 0x92, 0x28,
		0x00, 0x12, 0x17, 0x91, 0x92, 0x28, 0x60, 0x23, 0x06, 0x01, 0x02, 0x00, 0xFF, 0x11, 0x04, 0x00,
		0x00, 0x01	
	};
	
	int i,k;
	unsigned short arm7s,arm7e;
	unsigned short ts;
	
	// NDS header is SEQ 0000
	// ARM9 Bin start at 0001
	// ARM7 bin start at arm7s
	
	// find the first seq# for ARM7 binary data...
	// should be (rsa->arm9size-(rsa->arm9size%504))/504;
	arm7s = (rsa->arm9size-(rsa->arm9size%504))/504;
	if ((rsa->arm9size%504) != 0) arm7s++;
	
	arm7s++;
	
	arm7e = (rsa->arm7size-(rsa->arm7size%504))/504;
	if ((rsa->arm7size%504) != 0) arm7e++;
	
	arm7e += arm7s;
	
	for(i=0;i<34;i++) {
		d[i] = head[i];
	}

	if (seq == 0) {
		// NDS header data
		// copy 0x160 bytes of data from file to data @ 0x22
		//RF_setpos(f,0);
		d[0x20] = 0; // always 0?

		// seq 0
		d[0x21] = 0;
		d[0x22] = 0; 

		// data size in words...
		d[0x1C] = 0xB3;

		/*
		//RF_read(&d[0x23], 0x160, f);
		for(k=0;k<0x160;k++) { 
			d[0x23+k] = data[k]; 
		}
		*/
		fseek(fileHandle, seq, SEEK_SET);
		fread((u8*)&d[0x23], 1, (int)0x160, fileHandle);
		
		d[0x23+0x160] = 0;
		d[0x23+0x161] = 2;
		d[0x23+0x162] = 0;
		return 0x23+0x163; //!? think thats right...
	}
	
	if (seq < arm7s) {
		// arm9 data...

		d[0x20] = 0; // always 0?

		// copy in seq #
		d[0x21] = (seq&0xFF);
		d[0x22] = (seq&0xFF00)>>8;

		if (((rsa->arm9size%504) == 0) || (seq != (arm7s-1))) {
			// size == 504 (FF payload size)
			d[0x1C] = 0xFF;
			//RF_setpos(f,ndsh->arm9romSource+((seq-1)*504));
			//RF_read(&d[0x23], 504, f);
			
			/*
			for(k=0;k<504;k++) { 
				d[0x23+k] = data[(ndsh->arm9romSource+((seq-1)*504))+k]; 
			}
			*/
			fseek(fileHandle, (ndsh->arm9romSource+((seq-1)*504)), SEEK_SET);
			fread((u8*)&d[0x23], 1, (int)504, fileHandle);
		
			d[0x23+504] = 0;
			d[0x23+505] = 2;
			d[0x23+506] = 0;			
			return 0x24+506;
		}
		else {
			// size is to be determined...
			ts = (rsa->arm9size%504);
			if (ts&1) ts++; // make it even
			//RF_setpos(f,ndsh->arm9romSource+((seq-1)*504));
			//RF_read(&d[0x23], ts, f);
			/*
			for(k=0;k<ts;k++) { 
				d[0x23+k] = data[(ndsh->arm9romSource+((seq-1)*504))+k]; 
			}*/
			fseek(fileHandle, (ndsh->arm9romSource+((seq-1)*504)), SEEK_SET);
			fread((u8*)&d[0x23], 1, (int)ts, fileHandle);
			
			d[0x1C] = (6+ts)/2;
			d[0x23+ts] = 0;
			d[0x23+ts+1] = 2;
			d[0x23+ts+2] = 0;
			return 0x24+ts+2;
		}
	}

	if ((seq >= arm7s) && (seq < arm7e)) {
		// arm7 binary stuff...

		d[0x20] = 0; // always 0?

		// copy in seq #
		d[0x21] = (seq&0xFF);
		d[0x22] = (seq&0xFF00)>>8;
                                       
		if (((rsa->arm7size%504) == 0) || (seq != (arm7e-1)))                                                                {
			// size == 504 (FF payload size)
			d[0x1C] = 0xFF;
			//RF_setpos(f,ndsh->arm7romSource+((seq-arm7s)*504));
			//RF_read(&d[0x23], 504, f);
			/*
			for(k=0;k<504;k++) { 
				d[0x23+k] = data[(ndsh->arm7romSource+((seq-arm7s)*504))+k]; 
			}*/
			fseek(fileHandle, (ndsh->arm7romSource+((seq-arm7s)*504)), SEEK_SET);
			fread((u8*)&d[0x23], 1, (int)504, fileHandle);
			
			d[0x23+504] = 0;
			d[0x23+505] = 2;
			d[0x23+506] = 0;			
			return 0x24+506;
		}
		else {
			// size is to be determined...
			ts = (rsa->arm7size%504);
			if (ts&1) ts++; // make it even
			//RF_setpos(f,ndsh->arm7romSource+((seq-arm7s)*504));
			//RF_read(&d[0x23], ts, f);
			/*
			for(k=0;k<ts;k++) { 
				d[0x23+k] = data[(ndsh->arm7romSource+((seq-arm7s)*504))+k]; 
			}
			*/
			fseek(fileHandle, (ndsh->arm7romSource+((seq-arm7s)*504)), SEEK_SET);
			fread((u8*)&d[0x23], 1, (int)ts, fileHandle);
			
			d[0x1C] = (6+ts)/2;
			d[0x23+ts] = 0;
			d[0x23+ts+1] = 2;
			d[0x23+ts+2] = 0;
			return 0x24+ts+2;
		}
	}

	// should never happen...
	return 0;

}

void Timer_Beacons() {
	SendBeacon(ad, bnum, 0);
	bnum++;
	if (bnum == 10) bnum = 0;
}

int CompareMAC(u8* a, u8* b){
	int i;
	for(i=0;i<6;i++) {
		if (a[i] != b[i]) {
			return 0;
		}
	}
	return 1;
}

int getRSASignatureOffsetFromFileHandle(FILE * fh){
	int offset = -1;
	int fLoc = ftell(fh);
	int fileSize = FS_getFileSizeFromOpenHandle(fh);
	#define bufferSize (int)(64*1024)
	#define RSASignaure (u32)(0x00016361)	//c a 
	u8* bufferSeek = (u8*)malloc(bufferSize);
	int i = 0;
	for(i = 0; i < (fileSize/bufferSize) + 1; i++){
		int read = fread(bufferSeek, 1, bufferSize, fh);
		int j = 0;
		for(j=0; j < (read/sizeof(u32)); j++){
			if((u32)*(u32*)(bufferSeek + (j*4)) == RSASignaure){
				offset+=((i*bufferSize) + (j*4) + 1);	//account the default invalid value
				break;
			}
		}
	}
	free(bufferSeek);
	fseek(fh, fLoc, SEEK_SET);
	return offset;
}

void WMB_Main() {
	int i, ds, j=0, k;
	unsigned char *datapkt;
	struct nds_rsaframe *rsa;
	static FILE * ndsBinary = NULL;	//unsigned char *nds_data;
	tNDSHeader *nds_head;
	tNDSBanner *banner;
	unsigned char *d,*dr;
	struct iee80211_framehead *fh;
	bool gotrsa=0;
	int mode = 1;
	unsigned short curdata=0;
	unsigned char *dataacked;
	
	printf("* WMB Host - Version 0.1 *");
	printf("* By Juglak              *");
	
	printf("* Initializing wifi...");
	InitWifiMem();
	
	while(WIFI_READY != 1) swiDelay(10000);
	
	printf("done.");

	GetMAC(); 
	printf("* MAC: %2.2X %2.2X %2.2X %2.2X %2.2X", my_mac[0], my_mac[1], my_mac[2], my_mac[3], my_mac[4], my_mac[5] ); 
	Wifi_SetChannel9Juglak(13);
	
	if (my_mac[1] == 0) {
		printf("* ERROR: Invalid MAC Address ");
		while(1);
	}

	printf("* Testing wifi...");
	SendACK(my_mac);
	SendACK(my_mac);
	SendACK(my_mac);
	
	for(i=0;i<166;i++) {
		swiDelay(999);
	}
	
	while (tx_count != 0) {
		//printf("error.* Wifi was not initialized. * Please reset the system ");
		//while(1);
	}
	
	clrscr();
	printf("* - ");
	printf("* Press (A) to load a file from SLOT-1. ");
	printf("* There are no memory constraints. ");
	
	printf("* Press (B) to continue. ");
	
	while(1==1){
		scanKeys();
		if (keysPressed() & KEY_B){
			while(keysPressed() & KEY_B){
				scanKeys();
			}
			break;
		}
	}
	
	if(ndsBinary != NULL){
		fclose(ndsBinary);
	}
	
	char startPath[MAX_TGDSFILENAME_LENGTH+1];
	strcpy(startPath,"/");
	while( ShowBrowser((char *)startPath, (char *)&curChosenBrowseFile[0]) == true ){	//as long you keep using directories ShowBrowser will be true
		
	}
	
	ndsBinary = fopen(curChosenBrowseFile, "r");	//nds_data = (u8*)R_LoadFile(curChosenBrowseFile, NULL);
	
	printf("* Initializing data...");
	dataacked = (unsigned char *) Xcalloc(65535,1);
	datapkt = (unsigned char *) Xcalloc(2048,1);

	rsa = (struct nds_rsaframe *) Xcalloc(sizeof(struct nds_rsaframe),1);
	bool isRSASigned = InitRSA(rsa, ndsBinary);
	if(isRSASigned == true){
		printf("RSA-Signature detected! rsa addr: %x", rsa);
		//initGDBSession();
	}
	else{
		printf("Missing RSA-Signature!");
	}
	
	/*
	while(1==1){
		//GDB Stub Process must run here
		int retGDBVal = remoteStubMain();
		if(retGDBVal == remoteStubMainWIFINotConnected){
			if (switch_dswnifi_mode(dswifi_gdbstubmode) == true){
				//Show IP and port here
				printf("[Connect to GDB]: %s", ((getValidGDBMapFile() == true) ? " GDBFile Mode!" : "NDSMemory Mode!"));
				char IP[16];
				printf("Port:%d GDB IP:%s",remotePort, print_ip((uint32)Wifi_GetIP(), IP));
				remoteInit();
			}
			else{
				//GDB Client Reconnect:ERROR
			}
		}
		else if(retGDBVal == remoteStubMainWIFIConnectedGDBDisconnected){
			setWIFISetup(false);
			printf("Remote GDB Client disconnected. ");
			printf("Press A to retry this GDB Session. ");
			printf("Press B to reboot NDS GDB Server ");
			
			int keys = 0;
			while(1){
				scanKeys();
				keys = keysPressed();
				if (keys&KEY_A){
					break;
				}
				if (keys&KEY_B){
					break;
				}
				IRQVBlankWait();
			}
			
			if (keys&KEY_B){
				setValidGDBMapFile(false);
				main(0, (sint8**)"");
			}
			
			if (switch_dswnifi_mode(dswifi_gdbstubmode) == true){ // gdbNdsStart() called
				reconnectCount++;
				clrscr();
				//Show IP and port here
				printf("    ");
				printf("    ");
				printf("[Re-Connect to GDB]: %s",((getValidGDBMapFile() == true) ? " GDBFile Mode!" : "NDSMemory Mode!"));
				char IP[16];
				printf("Retries: %d",reconnectCount);
				printf("Port:%d GDB IP:%s", remotePort, print_ip((uint32)Wifi_GetIP(), IP));
				remoteInit();
			}
		}
	}
	*/
	
	ad = (struct ds_advert *) Xcalloc(sizeof(struct ds_advert),1);
	banner = (tNDSBanner *) Xcalloc(sizeof(tNDSBanner),1);
	
	ad->unk220 = 0x0B;
	ad->max_players = 1;
	
	// copy DS name into AD
	struct sIPCSharedTGDS * TGDSIPC = TGDSIPCStartAddress;
	int nameLen = (int)(TGDSIPC->DSFWSETTINGSInst.nickname_length_chars[0] | TGDSIPC->DSFWSETTINGSInst.nickname_length_chars[1] << 8);	//01Ah  2   Nickname length in characters    (0..10)
	ad->hostname_len = nameLen;
	for(i=0;i<ad->hostname_len;i++) {
		ad->hostname[i] = TGDSIPC->DSFWSETTINGSInst.nickname_utf16[i];
	}
	
	// make a description...
	sprintf(ts,"TGDS: WMB Host v0.1");
	for(i=0;(unsigned)i<strlen(ts);i++) {
		ad->game_description[i] = ts[i];
	}
	
	nds_head = (tNDSHeader *) Xcalloc(sizeof(tNDSHeader),1);
	
	d = (unsigned char *) (unsigned int) nds_head;
	
	//read NDSHeader
	/*
	for(i=0; (unsigned)i<sizeof(tNDSHeader);i++){ 
		d[i] = nds_data[i];
	}
	*/
	fseek(ndsBinary, 0, SEEK_SET);
	fread(d, 1, (int)sizeof(tNDSHeader), ndsBinary);
	
	rsa->arm9destTemp = nds_head->arm9destination;
	rsa->arm9destActual = nds_head->arm9destination;	
	rsa->arm9exeaddr = nds_head->arm9executeAddress;
	rsa->arm7exeaddr = nds_head->arm7executeAddress;
	rsa->arm9size = nds_head->arm9binarySize;
	rsa->arm7destActual = nds_head->arm7destination;
	rsa->arm7size = nds_head->arm7binarySize;
	
	d = (unsigned char *) (unsigned int) rsa;
	for(i=0;(unsigned)i<sizeof(struct nds_rsaframe);i++) {
		rsapkt[0x1F+i] = d[i];
	}
	
	// setup demo title and icon...
	sprintf(ts,"DLP: %s", curChosenBrowseFile);
	for(i=0;(unsigned)i<strlen(ts);i++) {
		ad->game_name[i] = ts[i];
	}
	
	// and a blue test icon...
	ad->icon_pallete[0] = 1;
	ad->icon_pallete[1] = RGB15(0,0,31) | 0x8000;
	for(i=0;i<512;i++) ad->icon[i] = 0x11;

	Wifi_SetChannel9Juglak(WMB_CHANNEL);
	TIMERXDATA(2) = (unsigned short) TIMER_FREQ_64(8);
	TIMERXCNT(2) = TIMER_DIV_64 | TIMER_ENABLE | TIMER_IRQ_REQ;
	EnableIrq(IRQ_TIMER2);
	
	setKeys(KEY_X);	//Press X
	
	while(1) {
		scanKeys();
		if (keysPressed() & KEY_X) {
			mode = 1;
			clrscr();
			printf("    ");
			printf("    ");
			printf("* ARM9/7 Size: %d/%d *", rsa->arm9size, rsa->arm7size);
			printf("* Using channel %d * Host ready! ", WMB_CHANNEL);
			printf(" -------Press X to Reset Host -------- ");	
			printf(" -------PRESS B to return -------- ");	
		}
		
		if (keysPressed() & KEY_B) {
			break;
		}
		
		if (mode == 1) {
			Wifi_SetRetry(4);
			dr = RXNextFrame(&ds);
			if (dr != NULL) {
				fh = (struct iee80211_framehead *) (unsigned int) dr;
					
				// check against macs later to see if its really to us
				
				if (((FH_FC_TYPE(fh->frame_control) == 0) && (FH_FC_SUBTYPE(fh->frame_control) == 11)) && CompareMAC(my_mac, fh->mac3)) {
	
					printf(" Received Authentication ");
					//printf("\x1b[19;0HMAC1: "); for(i=0;i<6;i++) printf("%2.2X ",fh->mac1[i]);
					//printf("\x1b[20;0HMAC2: "); for(i=0;i<6;i++) printf("%2.2X ",fh->mac2[i]);
					//printf("\x1b[21;0HMAC3: "); for(i=0;i<6;i++) printf("%2.2X ",fh->mac3[i]);
					//printf("\x1b[21;0HMAC3: "); for(i=0;i<6;i++) printf("%2.2X ",fh->mac3[i]);
						
					// ack it
					SendACK(&fh->mac2[0]);
						
					copy_mac(&authres[4], &fh->mac2[0]);
					copy_mac(&authres[10], &mymac[0]);
					copy_mac(&authres[16], &mymac[0]);
					fh = (struct iee80211_framehead *) (unsigned int) &authres[0];
					fh->sequence_control = bec_seq<<4;
					bec_seq++;				
					SendFrame(authres,30);
				}
				if (((FH_FC_TYPE(fh->frame_control) == 0) && (FH_FC_SUBTYPE(fh->frame_control) == 0)) && CompareMAC(my_mac, fh->mac3)) {
					printf(" Received Association Request .");
					//printf("\x1b[19;0HMAC1: "); for(i=0;i<6;i++) printf("%2.2X ",fh->mac1[i]);
					//printf("\x1b[20;0HMAC2: "); for(i=0;i<6;i++) printf("%2.2X ",fh->mac2[i]);
					//printf("\x1b[21;0HMAC3: "); for(i=0;i<6;i++) printf("%2.2X ",fh->mac3[i]);
					
					SendACK(&fh->mac2[0]);
					
					copy_mac(&assocres[4], &fh->mac2[0]);
					copy_mac(&assocres[10], &mymac[0]);
					copy_mac(&assocres[16], &mymac[0]);
					fh = (struct iee80211_framehead *) (unsigned int) &assocres[0];
					fh->sequence_control = bec_seq<<4;
					bec_seq++;
					SendFrame(assocres,34); // 28+29 = AID
					mode = 2;
					gotrsa = 0;
					j = 0;
						
				}
			}
		}
			
		if (mode == 2) {
			// we're sending pings out... then RSA frame...
			Wifi_SetRetry(0);
			
			if ((j<40) || (gotrsa)) {
				// send pings and stuff
				copy_mac(&pingdata[10], &mymac[0]);
				copy_mac(&pingdata[16], &mymac[0]);
				fh = (struct iee80211_framehead *) (unsigned int) &pingdata[0];
				fh->sequence_control = bec_seq<<4;
				bec_seq++;
				
				SendFrame(pingdata, 38);
			}
			else{
				// send RSA thing
				copy_mac(&rsapkt[10], &mymac[0]);
				copy_mac(&rsapkt[16], &mymac[0]);
				fh = (struct iee80211_framehead *) (unsigned int) &rsapkt[0];
				fh->sequence_control = bec_seq<<4;
				bec_seq++;
				
				SendFrame(rsapkt, 0x10A);
				//printf("Sent RSA Frame... ");					
			}
			
			swiDelay(11111);
			
			dr = RXNextFrame(&ds);
			if (dr != NULL) {
				fh = (struct iee80211_framehead *) (unsigned int) dr;
				if (FH_FC_TYPE(fh->frame_control) == 2) {
					// got DS response...
					if ((FH_FC_SUBTYPE(fh->frame_control) == 1) || (FH_FC_SUBTYPE(fh->frame_control) == 5)) {
						// probably a response from the DS...
						if (dr[0x15] == 0x10) {
							if (((FH_FC_SUBTYPE(fh->frame_control) == 1) && (dr[0x1A] == 0x08)) && CompareMAC(my_mac, fh->mac1)) {
								// rsa ACK
								gotrsa = 1;
								printf("DS ACKED RSA... ");
							}
						}
					}
				}
			}
			// wait for response from ds... or vblank?
			vblank_happened = false;
				
			
			// send DS ack thing
			copy_mac(&ackdata[10], &mymac[0]);
			copy_mac(&ackdata[16], &mymac[0]);
			fh = (struct iee80211_framehead *) (unsigned int) &ackdata[0];
			fh->sequence_control = bec_seq<<4;
			bec_seq++;
			ackdata[24] = rx_base % 256; // random...
			SendFrame(ackdata, 28);
			
			if (gotrsa == 1) {
				// data mode?
				mode = 3;
				Xfree(dataacked);
				dataacked = (unsigned char *) Xcalloc(65535,1);
				curdata = 1;
			}
			
			j++;

		}
		
		if (mode == 3) {
			Wifi_SetRetry(0);

			// data transfer...
			// send data packet curdata... wait for ack
			k = BuildNDSData(&datapkt[0], curdata, rsa, nds_head, (FILE*)ndsBinary);
			copy_mac(&datapkt[10], &mymac[0]);
			copy_mac(&datapkt[16], &mymac[0]);
			fh = (struct iee80211_framehead *) (unsigned int) &datapkt[0];
			fh->sequence_control = bec_seq<<4;
			bec_seq++;
			SendFrame(datapkt, k);

			// send DS ack thing
			swiDelay(2097); // 0.25 ms delay // could probably be tweaked...
			copy_mac(&ackdata[10], &mymac[0]);
			copy_mac(&ackdata[16], &mymac[0]);
			fh = (struct iee80211_framehead *) (unsigned int) &ackdata[0];
			fh->sequence_control = bec_seq<<4;
			bec_seq++;
			ackdata[24] = tx_base % 256;
			SendFrame(ackdata, 28);
			j++;
			
			dr = RXNextFrame(&ds);
			if (dr != NULL) {
				fh = (struct iee80211_framehead *) (unsigned int) dr;
				if (FH_FC_TYPE(fh->frame_control) == 2) {
					// got DS response...
					if ((FH_FC_SUBTYPE(fh->frame_control) == 1) || (FH_FC_SUBTYPE(fh->frame_control) == 5)) {
						// probably a response from the DS...
						if (dr[0x15] == 0x10) {
							if (((FH_FC_SUBTYPE(fh->frame_control) == 1) && (dr[0x1A] == 0x09)) && CompareMAC(my_mac, fh->mac1)) {
								// data ACK
								printfCoords(0, 6, "DS ACKED DATA %x... ", (unsigned short)(dr[0x1b]+(dr[0x1c]*256)));
								curdata = (unsigned short)(dr[0x1b]+(dr[0x1c]*256));
							}
							if (((FH_FC_SUBTYPE(fh->frame_control) == 1) && (dr[0x1A] == 0x0a)) && CompareMAC(my_mac, fh->mac1)) {
								// download complete?!
								mode=4;
							}
							
						}
					}
				}
			}
			
		}
		
		if (mode == 4) {
			Wifi_SetRetry(0);
			
			copy_mac(&enddata[10], &mymac[0]);
			copy_mac(&enddata[16], &mymac[0]);
			fh = (struct iee80211_framehead *) (unsigned int) &enddata[0];
			fh->sequence_control = bec_seq<<4;
			bec_seq++;
				
			SendFrame(enddata, 38);
			
			swiDelay(1000);
			dr = RXNextFrame(&ds);
			if (dr != NULL) {
				fh = (struct iee80211_framehead *) (unsigned int) dr;

				if (FH_FC_TYPE(fh->frame_control) == 2) {
					// got DS response...
					if (((FH_FC_SUBTYPE(fh->frame_control) == 1) || (FH_FC_SUBTYPE(fh->frame_control) == 5))  && CompareMAC(my_mac, fh->mac1)) {
						// probably a response from the DS...
						if (dr[0x15] == 0x10) {
						}
					}
				}
				if (FH_FC_TYPE(fh->frame_control) == 0) {
					if ((FH_FC_SUBTYPE(fh->frame_control) == 12) && CompareMAC(my_mac, fh->mac1)) {
						printf("Received Deauthentication .");
						swiDelay(7000);
						// YAY! DEAUTH!
						SendACK(&fh->mac2[0]);
						swiDelay(7000);
						mode=1;
					}
				}
			}
			// send DS ack thing
			copy_mac(&ackdata[10], &mymac[0]);
			copy_mac(&ackdata[16], &mymac[0]);
			fh = (struct iee80211_framehead *) (unsigned int) &ackdata[0];
			fh->sequence_control = bec_seq<<4;
			bec_seq++;
			ackdata[24] = tx_base % 256;
			SendFrame(ackdata, 28);
			j++;
			
		}
		
	}	
}

/* Juglak ARM9 Code end */

bool GDBEnabled = false;
char curChosenBrowseFile[MAX_TGDSFILENAME_LENGTH+1];

void menuShow(){
	clrscr();
	printf("                              ");
	printf(" Button (Start): File browser -> Sel. File (A): ");
	printf(" Host a DownLoad Play to external DS! ");
	
	printf("Available heap memory: %d", getMaxRam());
	printf("Button (Select): this menu. ");
}

int main(int _argc, sint8 **_argv) {
	
	/*			TGDS 1.5 Standard ARM9 Init code start	*/
	bool project_specific_console = true;	//set default console or custom console: custom console
	GUI_init(project_specific_console);
	GUI_clear();
	
	sint32 fwlanguage = (sint32)getLanguage();
	printf("     ");
	#ifdef ARM7_DLDI
	setDLDIARM7Address((u32 *)TGDSDLDI_ARM7_ADDRESS);	//Required by ARM7DLDI!
	#endif
	
	int ret=FS_init();
	if (ret == 0)
	{
		printf("FS Init ok.");
	}
	else if(ret == -1)
	{
		printf("FS Init error.");
	}
	//switch_dswnifi_mode(dswifi_idlemode);
	/*			TGDS 1.5 Standard ARM9 Init code end	*/
	
	//load TGDS Logo (NDS BMP Image)
	//VRAM A Used by console
	//VRAM C Keyboard and/or TGDS Logo
	
	//render TGDSLogo from a LZSS compressed file
	RenderTGDSLogoSubEngine((uint8*)&TGDSLogoLZSSCompressed[0], TGDSLogoLZSSCompressed_size);

	// init a simple RIPC for ARM9<->ARM7 conversations
	// use libriyo IPC3 mp2 regs
	RIPC = (unsigned int *) (unsigned int)((unsigned int)&RIPCc[0] | 0x00400000);
	RIPC[0] = 0;
	IPC3->mp2size = (unsigned int) &RIPC[0];
	IPC3->t1 = 0x12345678;
	while(IPC3->t1 != 0x23456789);
	
	//Init XMEM (let's see how good this one behaves...)
	u32 xmemsize = XMEMTOTALSIZE;
	xmemsize = xmemsize - (xmemsize/XMEM_BS) - 1024;
	xmemsize = xmemsize - (xmemsize%1024);
	XmemSetup(xmemsize, XMEM_BS);
	XmemInit();
	
	menuShow();
	
	while (1){
		scanKeys();
		if (keysPressed() & KEY_SELECT){
			menuShow();
			scanKeys();
			while(keysPressed() & KEY_SELECT){
				scanKeys();
			}
		}
		
		if (keysPressed() & KEY_START){
			WMB_Main();
			menuShow();
			scanKeys();
			while(keysPressed() & KEY_START){
				scanKeys();
			}
		}
		
		//GDB Debugging start
		//#ifdef NDSGDB_DEBUG_ENABLE
		if(GDBEnabled == true){
			//GDB Stub Process must run here
			int retGDBVal = remoteStubMain();
			if(retGDBVal == remoteStubMainWIFINotConnected){
				if (switch_dswnifi_mode(dswifi_gdbstubmode) == true){
					//clrscr();
					//Show IP and port here
					//printf("    ");
					//printf("    ");
					printf("[Connect to GDB]: %s", ((getValidGDBMapFile() == true) ? " GDBFile Mode!" : "NDSMemory Mode!"));
					char IP[16];
					printf("Port:%d GDB IP:%s",remotePort, print_ip((uint32)Wifi_GetIP(), IP));
					remoteInit();
				}
				else{
					//GDB Client Reconnect:ERROR
				}
			}
			else if(retGDBVal == remoteStubMainWIFIConnectedGDBDisconnected){
				setWIFISetup(false);
				//clrscr();
				//printf("    ");
				//printf("    ");
				printf("Remote GDB Client disconnected. ");
				printf("Press A to retry this GDB Session. ");
				printf("Press B to reboot NDS GDB Server ");
				
				int keys = 0;
				while(1){
					scanKeys();
					keys = keysPressed();
					if (keys&KEY_A){
						break;
					}
					if (keys&KEY_B){
						break;
					}
					IRQVBlankWait();
				}
				
				if (keys&KEY_B){
					setValidGDBMapFile(false);
					main(0, (sint8**)"");
				}
				
				if (switch_dswnifi_mode(dswifi_gdbstubmode) == true){ // gdbNdsStart() called
					reconnectCount++;
					clrscr();
					//Show IP and port here
					printf("    ");
					printf("    ");
					printf("[Re-Connect to GDB]: %s",((getValidGDBMapFile() == true) ? " GDBFile Mode!" : "NDSMemory Mode!"));
					char IP[16];
					printf("Retries: %d",reconnectCount);
					printf("Port:%d GDB IP:%s", remotePort, print_ip((uint32)Wifi_GetIP(), IP));
					remoteInit();
				}
			}
			
			//GDB Debugging end
			//#endif
		}	
		
	}
}