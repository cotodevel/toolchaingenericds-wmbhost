#include <stdlib.h>

#include "typedefsTGDS.h"
#include "dsregs.h"
#include "dsregs_asm.h"
#include "wifi_arm7.h"
#include "biosTGDS.h"

#define		SPI_CR		(*((u16 volatile *) 0x040001C0))
#define		SPI_DATA	(*((u16 volatile *) 0x040001C2))

#define WIFI_REG(ofs) (*((volatile u16 *)(0x04800000+(ofs))))
#define W_MACADDR		(((volatile u16 *)(0x04800018)))
#define W_RETRLIMIT		(*((volatile u16 *)(0x0480002C)))
#define W_IF			(*((volatile u16 *)(0x04800010)))
#define W_IE			(*((volatile u16 *)(0x04800012)))
#define W_MODE_WEP		(*((volatile u16 *)(0x04800006)))
#define W_BBSIOCNT		(*((volatile u16 *)(0x04800158)))
#define W_BBSIOWRITE	(*((volatile u16 *)(0x0480015A)))
#define W_BBSIOREAD		(*((volatile u16 *)(0x0480015C)))
#define W_BBSIOBUSY		(*((volatile u16 *)(0x0480015E)))
#define W_RFSIODATA2	(*((volatile u16 *)(0x0480017C)))
#define W_RFSIODATA1	(*((volatile u16 *)(0x0480017E)))
#define W_RFSIOBUSY		(*((volatile u16 *)(0x04800180)))
#define W_BSSID			(((volatile u16 *)(0x04800020)))
#define W_AIDS			(*((volatile u16 *)(0x04800028)))
#define W_POWERSTATE	(*((volatile u16 *)(0x0480003C)))

unsigned char MacAddr[6];	//DSWIFI inits this inside a struct, but Juglak uses it directly
//char FlashData[512];
//int MAC_Reglist[] = { 0x04, 0x08, 0x0A, 0x12, 0x10, 0x254, 0xB4, 0x80, 0x2A, 0x28, 0xE8, 0xEA, 0xEE, 0xEC, 0x1A2, 0x1A0, 0x110, 0xBC, 0xD4, 0xD8, 0xDA, 0x76 };
//int MAC_Vallist[] = { 0, 0, 0, 0, 0xffff, 0, 0xffff, 0, 0, 0, 0, 0, 1, 0x3F03, 1, 0, 0x0800, 1, 3, 4, 0x0602, 0};
//int RF_Reglist[] = { 0x146, 0x148, 0x14A, 0x14C, 0x120, 0x122, 0x154, 0x144, 0x130, 0x132, 0x140, 0x142, 0x38, 0x124, 0x128, 0x150 };

//the commented out vars/methods are identical to the DSWIFI ARM7 parts in TGDS
//the custom var/methods are renamed to JuglakX (X = ARM7 or ARM9)
/*
int Wifi_BBRead(int a) {
	while(W_BBSIOBUSY&1);
	W_BBSIOCNT=a | 0x6000;
	while(W_BBSIOBUSY&1);
	return W_BBSIOREAD;
}

int Wifi_BBWrite(int a, int b) {
	int i;
	i=0x2710;
	while((W_BBSIOBUSY&1)) {
		if(!i--) return -1;
	}
	W_BBSIOWRITE=b;
	W_BBSIOCNT=a | 0x5000;
	i=0x2710;
	while((W_BBSIOBUSY&1)) {
		if(!i--) return 0;
	}
	return 0;
}

void Wifi_RFWrite(int writedata) {
	while(W_RFSIOBUSY&1);
	W_RFSIODATA1=writedata;
	W_RFSIODATA2=writedata>>16;
	while(W_RFSIOBUSY&1);
}
*/


void Read_Flash(int address, char * destination, int length) {
	int i;
	while(SPI_CR&0x80);
	SPI_CR=0x8900;
	SPI_DATA=3;
	while(SPI_CR&0x80);
	SPI_DATA=(address>>16)&255;
	while(SPI_CR&0x80);
	SPI_DATA=(address>>8)&255;
	while(SPI_CR&0x80);
	SPI_DATA=(address)&255;
	while(SPI_CR&0x80);
	for(i=0;i<length;i++) {
		SPI_DATA=0;
		while(SPI_CR&0x80);
		destination[i]=SPI_DATA;
	}
	SPI_CR=0;
}

/*
void InitFlashData() {
	Read_Flash(0,FlashData,512);
}

int ReadFlashByte(int address) {
	if(address<0 || address>511) return 0;
	return FlashData[address];		
}

int ReadFlashBytes(int address, int numbytes) {
	int dataout=0;
	int i;
	for(i=0;i<numbytes;i++) {
		dataout |= ReadFlashByte(i+address)<<(i*8);
	}
	return dataout;
}
int ReadFlashHWord(int address) {
	if(address<0 || address>510) return 0;
	return ReadFlashBytes(address,2);
}

void Wifi_BBInit() {
	int i;
	WIFI_REG(0x160)=0x0100;
	for(i=0;i<0x69;i++) {
		Wifi_BBWrite(i,ReadFlashByte(0x64+i));
	}
}
*/

void Wifi_RFInitJuglak() {
	int i,j;
	int channel_extrabits;
	int numchannels;
	int channel_extrabytes;
	for(i=0;i<16;i++) {
		WIFI_REG(RF_Reglist[i])=ReadFlashHWord(0x44+i*2);
	}
	numchannels=ReadFlashByte(0x42);
	channel_extrabits=ReadFlashByte(0x41);
	channel_extrabytes=(channel_extrabits+7)/8;
	WIFI_REG(0x184)=((channel_extrabits>>7)<<8) | (channel_extrabits&0x7F);
	j=0xCE;
	if(ReadFlashByte(0x40)==3) {
		for(i=0;i<numchannels;i++) {
			Wifi_RFWrite(ReadFlashByte(j++)|(i<<8)|0x50000);
		}
	} else {
		for(i=0;i<numchannels;i++) {
			Wifi_RFWrite(ReadFlashBytes(j,channel_extrabytes));
			j+=channel_extrabytes;
		}
	}
}

void Wifi_StopJuglak() {
	W_IE=0;
	WIFI_REG(0x8004) = 0;
	WIFI_REG(0x80EA) = 0;
	WIFI_REG(0x80E8) = 0;
	WIFI_REG(0x8008) = 0;
	WIFI_REG(0x800A) = 0;

	WIFI_REG(0x80AC) = 0xFFFF;
	WIFI_REG(0x80B4) = 0xFFFF;
}

void Wifi_SetChannel7Juglak(int channel) {
	int i,n;
	if(channel<1 || channel>13) return;
    channel-=1;

    switch(ReadFlashByte(0x40)) {
    case 2:
    case 5:
        Wifi_RFWrite(ReadFlashBytes(0xf2+channel*6,3));
        Wifi_RFWrite(ReadFlashBytes(0xf5+channel*6,3));
        for(i=0;i<20000;i++) i++;
        Wifi_BBWrite(0x1E, ReadFlashByte(0x146+channel));
        break;
    case 3:
        n=ReadFlashByte(0x42);
        n+=0xCF;
        for(i=0;i<=ReadFlashByte(0x43);i++) {
            Wifi_BBWrite(ReadFlashByte(n),ReadFlashByte(n+channel+1));
            n+=15;
        }
        for(i=0;i<ReadFlashByte(0x43);i++) {
            Wifi_RFWrite( (ReadFlashByte(n)<<8) | ReadFlashByte(n+channel+1) | 0x050000 );
            n+=15;
        }
        break;
    default:
        break;
    }
	//WifiData->curChannel=channel+1;
}

/*
void Wifi_SetWepMode(int wepmode) {
	if(wepmode<0 || wepmode>7) return;
   if(wepmode==0) {
      WIFI_REG(0x32)=0x0000;
   } else {
      WIFI_REG(0x32)=0x8000;
   }
   if(wepmode==0) wepmode=1;
	W_MODE_WEP = (W_MODE_WEP & 0xFFC7) | (wepmode<<3);
}

void Wifi_SetMode(int wifimode) {
	if(wifimode>3 || wifimode<0) return;
	W_MODE_WEP = (W_MODE_WEP& 0xfff8) | wifimode;
}

void Wifi_SetPreambleType(int preamble_type) {
	if(preamble_type>1 || preamble_type<0) return;
	WIFI_REG(0x80BC) = (WIFI_REG(0x80BC) & 0xFFBF) | (preamble_type<<6);
}

void Wifi_DisableTempPowerSave() {
	WIFI_REG(0x8038) &= ~2;
	WIFI_REG(0x8048) = 0;
}

*/

void Wifi_WakeUpJuglak7() {
	u32 i;
	WIFI_REG(0x8036)=0;
	for(i=0;i<100000;i++) i++;
	WIFI_REG(0x8168)=0;

	i=Wifi_BBRead(1);
	Wifi_BBWrite(1,i&0x7f);
	Wifi_BBWrite(1,i);
	for(i=0;i<400000;i++) i++;

	Wifi_RFInitJuglak();
}

/*
void Wifi_Shutdown() {
	int a;
	if(ReadFlashByte(0x40)==2) {
		Wifi_RFWrite(0xC008);
	}
	a=Wifi_BBRead(0x1E);
	Wifi_BBWrite(0x1E,a|0x3F);
	WIFI_REG(0x168)=0x800D;
	WIFI_REG(0x36)=1;
}

void Wifi_MacInit() {
	int i;
	for(i=0;i<22;i++) {
		WIFI_REG(MAC_Reglist[i]) = MAC_Vallist[i];
	}
}
*/

void Wifi_StartJuglak7() {
	int i;
	Wifi_StopJuglak();

	WIFI_REG(0x8032) = 0x8000;
	WIFI_REG(0x8134) = 0xFFFF;
	WIFI_REG(0x802A) = 0;
	W_AIDS           = 0;
	WIFI_REG(0x80E8) = 1;
	WIFI_REG(0x8038) = 0x0000;
	WIFI_REG(0x20) = 0x0000;
	WIFI_REG(0x22) = 0x0000;
	WIFI_REG(0x24) = 0x0000;

	//Wifi_TxSetup();
	WIFI_REG(0x80AE)=0x000D;
	
	//Wifi_RxSetup();
	WIFI_REG(0x8030) = 0x8000;
	WIFI_REG(0x8050) = 0x4C00;
	WIFI_REG(0x8056) = 0x0600;
	WIFI_REG(0x8052) = 0x5F60;
	WIFI_REG(0x805A) = (WIFI_REG(0x8050)&0x3FFF)>>1;
	WIFI_REG(0x8062) = 0x5F5E;
	WIFI_REG(0x8030) = 0x8001;



	WIFI_REG(0x8030) = 0x8000;

	W_IF=0xFFFF;

	W_IE = 0;
		
	WIFI_REG(0x81AE)=0x1fff;
	WIFI_REG(0x81AA)=0;
	W_BSSID[0]=0xFFFF;
	W_BSSID[1]=0xFFFF;
	W_BSSID[2]=0xFFFF;
	WIFI_REG(0x80D0)=0x0181;
	WIFI_REG(0x80E0)=0x000D;
	WIFI_REG(0x8008)=0xe000;
	WIFI_REG(0x800A)=0;
	WIFI_REG(0x8004)=1;
	WIFI_REG(0x80E8)=1;
	WIFI_REG(0x80EA)=1;
	WIFI_REG(0x8012) = 0x703F;	
	
	WIFI_REG(0x8048)=0x0000;
	Wifi_DisableTempPowerSave();
	W_POWERSTATE |= 2;
	WIFI_REG(0xAC) = 0xFFFF;
	i=0xFA0;
	while(i!=0 && !(WIFI_REG(0x819C)&0x80)) i--;
}

void Wifi_initJuglak7() {
	int i;

	POWERCNT7 |= 2;
	*((volatile u16 *)0x04000206) = 0x30;

    InitFlashData();

	WIFI_REG(0x4)=0xffff;
	Wifi_StopJuglak();
	Wifi_Shutdown();
	
	for(i=0x4000;i<0x6000;i+=2) WIFI_REG(i)=0;

	for(i=0;i<6;i++)  MacAddr[i]=ReadFlashByte(0x36+i);

	W_IE=0;
	Wifi_WakeUpJuglak7();

	Wifi_MacInit();
	Wifi_RFInitJuglak();
	Wifi_BBInit();

	W_MACADDR[0]=((u16 *)MacAddr)[0];
	W_MACADDR[1]=((u16 *)MacAddr)[1];
	W_MACADDR[2]=((u16 *)MacAddr)[2];

	W_BSSID[0]=((u16 *)MacAddr)[0];
	W_BSSID[1]=((u16 *)MacAddr)[1];
	W_BSSID[2]=((u16 *)MacAddr)[2];
		
	W_RETRLIMIT=7;
	Wifi_SetMode(0);
	Wifi_SetWepMode(0);

	Wifi_SetChannel7Juglak(1);

	Wifi_BBWrite(0x13, 0x00);
	Wifi_BBWrite(0x35, 0x1F);	

	swiDelay(333);
	Wifi_StartJuglak7();
	swiDelay(333);
	swiDelay(333);
	
	Wifi_SetPreambleType( 1);
	
	W_IF=0;
	W_IE=1;
}

/*
void Wifi_MACWrite(u16 * src, u32 MAC_Base, u32 MAC_Offset, u32 length) {
	int endrange,subval;
	int thislength;
	endrange = (WIFI_REG(0x52)&0x1FFE);
	subval=((WIFI_REG(0x52)&0x1FFE)-(WIFI_REG(0x50)&0x1FFE));
	MAC_Base += MAC_Offset;
	if(MAC_Base>=endrange) MAC_Base -= subval;
	while(length>0) {
		thislength=length;
		if(length>(endrange-MAC_Base)) length=endrange-MAC_Base;
		length-=thislength;
		while(thislength>0) {
			WIFI_REG(0x4000+MAC_Base) = *(src++);
			MAC_Base+=2;
			thislength-=2;
		}
		MAC_Base-=subval;
	}
}
*/

void Wifi_TxRawJuglak7(u16 * data, int datalen) {
	datalen = (datalen+3)&(~3);
	Wifi_MACWrite(data, 0, 0, datalen);
	WIFI_REG(0xA8)=0x8000;
}

/*
int Wifi_TxCheck() {
	if(WIFI_REG(0xA8)&0x8000) return 0;
	return 1;
}
*/

/*
void Wifi_MACCopy(u16 * dest, u32 MAC_Base, u32 MAC_Offset, u32 length) {
	int endrange,subval;
	int thislength;
	endrange = (WIFI_REG(0x52)&0x1FFE);
	subval=((WIFI_REG(0x52)&0x1FFE)-(WIFI_REG(0x50)&0x1FFE));
	MAC_Base += MAC_Offset;
	if(MAC_Base>=endrange) MAC_Base -= subval;
	while(length>0) {
		thislength=length;
		if(thislength>(endrange-MAC_Base)) thislength=endrange-MAC_Base;
		length-=thislength;
		while(thislength>0) {
			*(dest++) = WIFI_REG(0x4000+MAC_Base);
			MAC_Base+=2;
			thislength-=2;
		}
		MAC_Base-=subval;
	}
}
*/
