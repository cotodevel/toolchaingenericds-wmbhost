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

#include "typedefsTGDS.h"
#include "dsregs.h"
#include "dsregs_asm.h"
#include "interrupts.h"
#include "biosTGDS.h"
#include "ipcfifoTGDSUser.h"
#include "spifwTGDS.h"
#include "wifi_arm7.h"
#include "main.h"
#include "InterruptsARMCores_h.h"

//User Handler Definitions

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void IpcSynchandlerUser(uint8 ipcByte){
	switch(ipcByte){
		default:{
			//ipcByte should be the byte you sent from external ARM Core through sendByteIPC(ipcByte);
		}
		break;
	}
}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void Timer0handlerUser(){

}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void Timer1handlerUser(){

}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void Timer2handlerUser(){

}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void Timer3handlerUser(){

}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void HblankUser(){

}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void VblankUser(){
	
}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void VcounterUser(){
	static int lastbut = -1;
	
	uint16 but=0, x=0, y=0, xpx=0, ypx=0, z1=0, z2=0;

	but = REG_KEYXY;

	if (!( (but ^ lastbut) & (1<<6))) {
		
		//XYReadScrPosUser(&tempPos);

		x = tempPos.rawx;
		y = tempPos.rawy;
		xpx = tempPos.px;
		ypx = tempPos.py;
		z1 = tempPos.z1;
		z2 = tempPos.z2;
		
	} else {
		lastbut = but;
		but |= (1 <<6);
	}

	if ( vcount == 80 ) {
		first = tempPos;
	} else {
		if (	abs( xpx - first.px) > 10 || abs( ypx - first.py) > 10 ||
				(but & ( 1<<6)) ) {

			but |= (1 <<6);
			lastbut = but;

		} else { 	
			//IPC->mailBusy = 1;
			//IPC->rawx			= x;
			//IPC->rawy			= y;
			//IPC->touchXpx		= xpx;
			//IPC->touchYpx		= ypx;
			//IPC->touchZ1		= z1;
			//IPC->touchZ2		= z2;
			//IPC->mailBusy = 0;
		}
	}
	//IPC->buttons		= but;
	vcount ^= (80 ^ 130);
	
	setVCountIRQLine(vcount);
	
	//ori removed: R_CmdHandler(1);
	RIPC_Cmd(1);
}

//Note: this event is hardware triggered from ARM7, on ARM9 a signal is raised through the FIFO hardware
#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void screenLidHasOpenedhandlerUser(){
	TurnOnScreens();
}

//Note: this event is hardware triggered from ARM7, on ARM9 a signal is raised through the FIFO hardware
#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void screenLidHasClosedhandlerUser(){
	TurnOffScreens();
}