/*
 * pic16/button.c
 */

#define __18F97J60
#define __SDCC__
#include "../Include/HardwareProfile.h"

#include "../platform/button.h"

BUTTON_HANDLER button0_handler = NULL;
BUTTON_HANDLER button1_handler = NULL;

void button_init(void) {
	BUTTON0_TRIS = 1; //configure button0 as input
	BUTTON1_TRIS = 1; //configure button1 as input

	INTCONbits.GIE     = 1;   //enable interrupts
	RCONbits.IPEN      = 1;   //enable interrupts priority levels

	//BUTTON 0
	INTCON3bits.INT1P  = 0;   //connect INT1 interrupt to low priority
	INTCON2bits.INTEDG1= 0;   //INT1 interrupts on falling edge
	INTCON3bits.INT1E  = 1;   //enable INT1 interrupt
	INTCON3bits.INT1F  = 0;   //clear INT1 flag

	//BUTTON1
	INTCON3bits.INT3P  = 0;   //connect INT3 interrupt to low priority
	INTCON2bits.INTEDG3= 0;   //INT3 interrupts on falling edge
	INTCON3bits.INT3E  = 1;   //enable INT3 interrupt
	INTCON3bits.INT3F  = 0;   //clear INT1 flag
}

BOOL button0_down() {
	return BUTTON0_IO == 0u;
}

BOOL button1_down() {
	return BUTTON1_IO == 0u;
}

void button_set_handler(BYTE buttonNumber, BUTTON_HANDLER handler) {
	switch(buttonNumber) {
	case 0:
		button0_handler = handler;
		break;
	case 1:
		button1_handler = handler;
		break;
	}
}

void button_handle_intterupt(){
	if(button_reset(0)){
		if(button0_handler) button0_handler();
	}
	if(button_reset(1))
		if(button1_handler)	button1_handler();
}

}

BOOL button_reset(BYTE buttonNumber) {
	switch(buttonNumber) {
	case 0:
		if(INTCON3bits.INT1F == 1){
			INTCON3bits.INT1F  = 0;
			return TRUE;
		}
		break;
	case 1:
		if(INTCON3bits.INT3F == 1){
			INTCON3bits.INT3F  = 0;
			return TRUE;
		}
		break;
	}
	return FALSE;
	// clear interrupt bits

}
