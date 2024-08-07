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
#include "biosTGDS.h"
#include "spifwTGDS.h"
#include "posixHandleTGDS.h"
#include "pff.h"
#include "ipcfifoTGDSUser.h"
#include "loader.h"
#include "dldi.h"
#include "exceptionTGDS.h"
#include "dmaTGDS.h"

////////////////////////////////TGDS-MB v3 VRAM Bootcode start////////////////////////////////
FATFS fileHandle;					// Petit-FatFs work area 
char fname[256];
u8 NDSHeaderStruct[4096];
char debugBuf7[256];

void bootfile(){
	
}

//ARM7 core specific bits
int vcount = 0;
struct touchPosition first, tempPos;
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
		
		handleARM7SVC();	/* Do not remove, handles TGDS services */
	}

}

bool GDBStarted = false;



#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
int main(int argc, char **argv) {
//---------------------------------------------------------------------------------
	/*			TGDS 1.6 Standard ARM7 Init code start	*/
	installWifiFIFO();
	while(!(*(u8*)0x04000240 & 2) ){} //wait for VRAM_D block
	ARM7InitDLDI(TGDS_ARM7_MALLOCSTART, TGDS_ARM7_MALLOCSIZE, TGDSDLDI_ARM7_ADDRESS);
	SendFIFOWords(FIFO_ARM7_RELOAD, 0xFF); //ARM7 Reload OK -> acknowledge ARM9
    /*			TGDS 1.6 Standard ARM7 Init code end	*/
	
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
	
	MyWifi(); //ARM7 Loop here
	return 0;
}

////////////////////////////////TGDS-MB v3 VRAM Bootcode end////////////////////////////////