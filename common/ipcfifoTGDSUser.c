
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

//TGDS required version: IPC Version: 1.3

//IPC FIFO Description: 
//		TGDSIPC 		= 	Access to TGDS internal IPC FIFO structure. 		(ipcfifoTGDS.h)
//		TGDSUSERIPC		=	Access to TGDS Project (User) IPC FIFO structure	(ipcfifoTGDSUser.h)

#include "ipcfifoTGDS.h"
#include "ipcfifoTGDSUser.h"
#include "dsregs.h"
#include "dsregs_asm.h"
#include "InterruptsARMCores_h.h"

#ifdef ARM7
#include <string.h>

#include "main.h"
#include "wifi_arm7.h"
#include "spifwTGDS.h"

#endif

#ifdef ARM9

#include <stdbool.h>
#include "main.h"
#include "wifi_arm9.h"

#endif

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void HandleFifoNotEmptyWeakRef(uint32 cmd1,uint32 cmd2){
	
	switch (cmd1) {
		//NDS7: 
		#ifdef ARM7
		case((u32)REQ_GBD_ARM7):{
			installWifiFIFO();
			GDBStarted = true;
		}
		break;
		case((u32)REQ_TX_FRAME):{
			
			uint32 * fifomsg = (uint32 *)&TGDSIPC->fifoMesaggingQueue[0];
			unsigned char *data = (unsigned char *)fifomsg[0]; 
			int len = (int)fifomsg[1];
			
			int i;
			for(i=0;i<12;i++) {
				tx_queue[tx_base][i] = 0;
			}

			tx_queue[tx_base][8] = 0x14; // Trans rate... a=1 14=2mbit
			
			// Data Size
			tx_queue[tx_base][10]	= (len+4)&0xFF;
			tx_queue[tx_base][11]	= ((len+4)&0xFF00)>>8;

			for(i=0;i<len;i++) {
				tx_queue[tx_base][i+12] = data[i];
			}

			tx_sizes[tx_base] = len+16;
			tx_base++;
			
			if(tx_base == 64) tx_base = 0;
			fifomsg[1] = fifomsg[0] = 0;
		}
		break;
		#endif
		
		//NDS9: 
		#ifdef ARM9
		
		#endif
	}
	
}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void HandleFifoEmptyWeakRef(uint32 cmd1,uint32 cmd2){
}

#ifdef ARM9
//Callback update sample implementation

__attribute__((section(".itcm")))
void updateSoundContextStreamPlaybackUser(u32 srcFrmt){
}
#endif

void setupSoundUser(u32 srcFrmtInst){
	#ifdef ARM7
	#endif
	
	#ifdef ARM9
	#endif
}

void stopSoundUser(u32 srcFrmt){
	#ifdef ARM7
	#endif
	
	#ifdef ARM9
	#endif
}

//project specific stuff
