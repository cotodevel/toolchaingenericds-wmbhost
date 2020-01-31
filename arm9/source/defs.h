#ifndef __wmbhostdefs_h__
#define __wmbhostdefs_h__

#define FH_FC_TYPE(x) ((unsigned char)((x & 0xC)>>2))
#define FH_FC_SUBTYPE(x) ((unsigned char)((x & 0xF0)>>4))

#define FH_SC_FRAGNUM(x) ((unsigned char)((x & 0xF)))
#define FH_SC_SEQNUM(x) ((unsigned short)((x & 0xFFF0)>>4))


// structs

struct iee80211_framehead {
	unsigned short frame_control;
	unsigned short duration_id;
	unsigned char mac1[6];
	unsigned char mac2[6];
	unsigned char mac3[6];
	unsigned short sequence_control;
	unsigned char mac4[6];
};

struct ds_advert {
	unsigned short icon_pallete[16];
	unsigned char icon[512];
	unsigned char unk220;
	unsigned char hostname_len;
	unsigned short hostname[10];
	unsigned char max_players;
	unsigned char unk237;
	unsigned short game_name[48];
	unsigned short game_description[96];
	//unsigned char unk358[64];
} __attribute__ ((__packed__));

struct beacon {
	unsigned char unk0[4];
	unsigned char destmac[6];
	unsigned char srcmac[6];
	unsigned char bssmac[6];
	unsigned short seq;
	unsigned int ts1; // ??? some timestamp
	unsigned int ts2; // ???
	unsigned short interval;
	unsigned short capability; // not sure if short or 2 bytes
	unsigned char tagparms[4]; // no idea
	unsigned char dsparms[3];
	unsigned char tidm[7];
	unsigned char vendor[2];
} __attribute__ ((__packed__));

struct ds_element {
	unsigned char manufacturer[3];
	unsigned char unk03;
	unsigned char unk04[4];
	unsigned char unk08[4];
	unsigned char unk0c[4];
	unsigned short streamcode;
	unsigned char unk12[2];
	unsigned char unk14[4]; 
	unsigned char unk18[4];
	unsigned char non_advert;
	unsigned char unk1D;
	unsigned char connected_clients;
	unsigned char sequence_number;
	unsigned short checksum;
	unsigned char advert_sequence_number;
	unsigned char advert_len; // in beacons
	unsigned short data_size;
	unsigned char data[256];
} __attribute__ ((__packed__));
  
struct nds_rsaframe {
	unsigned int arm9exeaddr;	//00h 4   ARM9 execute address
	unsigned int arm7exeaddr;	//04h 4   ARM7 execute address
	unsigned int zerofilled0; //08h 4   Zerofilled
	unsigned int headerDestTemp; //0Ch 4   Header destination (temp)
	unsigned int headerDestActual; //10h 4   Header destination (actual)
	unsigned int headerSize; //14h 4   Header size (160h)
	unsigned int zerofilled1; //18h 4   Zerofilled
	unsigned int arm9destTemp; 	//1Ch 4   ARM9 destination address (temp) 
	unsigned int arm9destActual; //20h 4   ARM9 destination address (actual)
	unsigned int arm9size; 	//24h 4   ARM9 binary size 
	unsigned int zerofilled2; 	//28h 4   Zerofilled
	unsigned int arm7destTemp; 	//2Ch 4   ARM7 destination address (temp) (usually 22C0000h in Main RAM)
	unsigned int arm7destActual; 	//30h 4   ARM7 destination address (actual) (usually somewhere in WRAM) 
	unsigned int arm7size; 	//34h 4   ARM7 binary size 
	unsigned int unk38; 	//38h 4   Unknown (00000001h)
	unsigned int signatureID; //3Ch 4   Signature ID (61h,63h,01h,00h) (aka "ac", or backwards "ca") ;
	unsigned char signatureRSA[0x80];//40h 80h Signature RSA (RSA signature in OpenPGP SHA1 format)  ;
	unsigned int signatureFooter;	//C0h 4   Signature Footer   
	unsigned char zerofilled3[36];//C4h 36  Zerofilled
	//E8h -   End of frame payload
} __attribute__ ((__packed__));


#endif
