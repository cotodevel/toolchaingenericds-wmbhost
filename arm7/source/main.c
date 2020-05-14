#include "main.h"
#include "typedefsTGDS.h"
#include "dsregs.h"
#include "biosTGDS.h"
#include "ipcfifoTGDSUser.h"
#include "spitscTGDS.h"
#include "InterruptsARMCores_h.h"
#include <stdlib.h>
#include "spifwTGDS.h"
#include "posixHandleTGDS.h"

//These buffers are project specific for ARM7 WAV SoundStream
u16 strpcmL0Buf[WAV_READ_SIZE];
u16 strpcmL1Buf[WAV_READ_SIZE];
u16 strpcmR0Buf[WAV_READ_SIZE];
u16 strpcmR1Buf[WAV_READ_SIZE];

int vcount = 0;

struct XYTscPos first, tempPos;	//touchPosition first, tempPos;

unsigned int *RIPC;

bool queues_setup=0;
unsigned char *tx_queue[64],*rx_queue[64];
unsigned int *rx_sizes,*tx_sizes;

unsigned char wifi_channel=6,setchan=0;
unsigned char wifi_retry=0;

void RIPC_Cmd(bool interupt) {
	// Process Local Code RIPC cmds
	if (RIPC[0] == 0) return;
	
	switch(RIPC[0]) {
		case 1: {
			// Ping from ARM9...
			RIPC[2] = ~RIPC[1];
			break;
		}
		
		case 2: {
			// Init queue memory...
			// RIPC[1] = pointer to ((64+64)*2048)+((64+64)*4) bytes of allocated main ram
			
			unsigned int i;
			
			// setup RX queue mem...
			for(i=0;i<64;i++) { rx_queue[i] = (unsigned char *) (unsigned int) ( RIPC[1] + (i*2048) ); }

			// setup TX queue mem...
			for(i=0;i<64;i++) { tx_queue[i] = (unsigned char *) (unsigned int) ( RIPC[1] + (i*2048) + 131072 ); }
			
			// setup RX queue sizes mem...
			rx_sizes = (unsigned int *) (unsigned int) ( RIPC[1] + 262144 );

			// setup RX queue sizes mem...
			tx_sizes = (unsigned int *) (unsigned int) ( RIPC[1] + 262144 + 256 );
			
			rx_base = 0;
			tx_base = 0;
			rx_count = 0;
			
			queues_setup = 1;
			
			break;
		}

		case 3: {
			// set a wifi reg from ARM9...
			WIFI_REG(RIPC[1]) = RIPC[3];
			break;
		}
		
		case 4: {
			// read a wifi reg
			RIPC[2] = WIFI_REG(RIPC[1]);
			break;
		}
		
		case 6: {
			// Set Channel
			wifi_channel = RIPC[1];
			break;
		}
		
		case 7: {
			// set retry count...
			wifi_retry = RIPC[1];
			break;
		}
		
		case 8: {
			// get mac address... kinda wasteful... but...
			RIPC[3] = MacAddr[0];
			RIPC[4] = MacAddr[1];
			RIPC[5] = MacAddr[2];
			RIPC[6] = MacAddr[3];
			RIPC[7] = MacAddr[4];
			RIPC[8] = MacAddr[5];
			break;
		}
		
		default: break;
	}
	
	RIPC[0] = 0;
}

#define BUF_START 0x4c00
#define BUF_END 0x5F60
#define BUF_SIZE (BUF_END - BUF_START)
#define BUF_CONTENTS_SIZE( write, read) ( (read) > (write) ? (BUF_SIZE - ((read) - (write))) : ( (write) - (read)))

int Juglak_WifiRX() {
	REG_IME = 0;
	
	int max_read_size, base;
	int write_ptr = WIFI_REG(0x8054) << 1;
	int read_ptr = WIFI_REG(0x805A) << 1;

	max_read_size = BUF_CONTENTS_SIZE( write_ptr, read_ptr);

	if ( max_read_size > 0) {
		base = rx_base;
		rx_sizes[base] = max_read_size;
		Wifi_MACCopy( (u16 *)rx_queue[base], read_ptr, 0, max_read_size);
	
		if (base == 63) {
			rx_base = 0; 
		} else { 
			rx_base = base + 1;
		}
		rx_count++;
		//WIFI_READY++;
	}

	WIFI_REG(0x805A) = write_ptr >> 1;
	REG_IME = 1;
	return 0;
}

void Juglak_WifiTXQueue() {
	if(Wifi_TxCheck() && (tx_base > 0)){
		WIFI_REG(0x802C) = wifi_retry;
		Wifi_TxRawJuglak7((unsigned short *)tx_queue[tx_base-1], tx_sizes[tx_base-1]);
		WIFI_REG(0x80AE) |= 0x0008;
		tx_base--;
	}
}

void MyWifi() {
	REG_IE&=~0x01000000;	//prevent wifi irqs to cause dswifi irq handler to takeover
	
	WIFI_REG(0x80D0) = 8095; // filter nothing important... ?
	WIFI_REG(0x8194) = 0x05; // true raw tx enable? (allows duration field on data packets to TX as specified)
	WIFI_REG(0x81A0) = 0x91D; // makes DS actually receive stuff!

	while(1) {
		// process local commands
		RIPC_Cmd(0);
		
		// process libriyo internal commands
		//R_CmdHandler(0);

		if (setchan != wifi_channel) Wifi_SetChannel7Juglak(wifi_channel);
		setchan = wifi_channel;

		Juglak_WifiRX();
		Juglak_WifiTXQueue();
	
		if(WIFI_REG(0x80A8)&0x8000) { 
			WIFI_REG(0x80AE)=0x000D; 
		}
		if(GDBStarted == true){
			break;
		}
	}

}

void initDLDIARM7(u32 srcDLDIAddr){	//stubbed
	
}

bool GDBStarted = false;

//---------------------------------------------------------------------------------
int main(int _argc, sint8 **_argv) {
//---------------------------------------------------------------------------------
	
	/*			TGDS 1.5 Standard ARM7 Init code start	*/
	//installWifiFIFO();		
	
	//wait for VRAM D to be assigned from ARM9->ARM7 (ARM7 has load/store on byte/half/words on VRAM)
	while (!(*((vuint8*)0x04000240) & 0x2));
	ARM7DLDIInit();
	
	int argBuffer[MAXPRINT7ARGVCOUNT];
	memset((unsigned char *)&argBuffer[0], 0, sizeof(argBuffer));
	writeDebugBuffer7("TGDS ARM7.bin Boot OK!", 0, (int)&argBuffer[0]);
	
	/*			TGDS 1.5 Standard ARM7 Init code end	*/
	
    //Set up PPU IRQ Vertical Line
	setVCountIRQLine(80);
	vcount = 80;
	
	// init RIPC...
	while(IPC3->t1 != 0x12345678);
	RIPC = (unsigned int *) (unsigned int) IPC3->mp2size;
	IPC3->t1 = 0x23456789;
	
	WIFI_READY = 0;
	// need the wifi rx/tx shared queue mem to be setup before we proceed...
	while(queues_setup==0) {
		swiDelay(10000);
		RIPC_Cmd(0);
	}
	
	Wifi_initJuglak7();	

	WIFI_READY = 1;
	GDBStarted = false;
	
	MyWifi(); //ARM7 Loop here. Unless GDBSession begins
	
	while (1) {
		handleARM7SVC();	/* Do not remove, handles TGDS services */
		IRQVBlankWait();
	}
   
	return 0;
}

//Custom Button Mapping Handler implementation: IRQ Driven
void CustomInputMappingHandler(uint32 readKeys){
	
}

//Project specific: ARM7 Setup for TGDS sound stream
void initSoundStreamUser(){
	//Buffers must be provided here. 
	//Format: s16 buffer[WAV_READ_SIZE];
	strpcmL0 = (s16*)&strpcmL0Buf[0];
	strpcmL1 = (s16*)&strpcmL1Buf[0];
	strpcmR0 = (s16*)&strpcmR0Buf[0];
	strpcmR1 = (s16*)&strpcmR1Buf[0];
}