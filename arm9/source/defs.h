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
	unsigned int arm9exeaddr;
	unsigned int arm7exeaddr;
	unsigned int unk08; // 0
	unsigned int headerdest; // 0x027FFE00
	unsigned int headerdest2; // 0x027FFE00
	unsigned int headersize; // 0x160
	unsigned int unk18; // 0
	unsigned int arm9dest; // 
	unsigned int arm9dest2; // 
	unsigned int arm9size; // 
	unsigned int unk28; // 0
	unsigned int unk2C; // 0x022C0000
	unsigned int arm7dest; // 
	unsigned int arm7size; // 
	unsigned int unk38; // 1
	unsigned char signature[136]; // 0's?
	unsigned char unkC4[36]; // 0's
} __attribute__ ((__packed__));


#endif
