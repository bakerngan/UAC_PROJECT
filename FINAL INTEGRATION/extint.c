/*****************************************************************************
 *   extint.c:  Target C file for Philips LPC214x Family Microprocessors
 *
 *   Copyright(C) 2006, Philips Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2005.10.01  ver 1.00    Prelimnary version, first Release
 *
*****************************************************************************/
#include "LPC24xx.h"                        /* LPC21xx definitions */
#include "type.h"
#include "irq.h"
#include "extint.h"
#include "target.h"
#include "uart.h"
#include "monitor.h"
#include "spi.h"
#include "FAT.h"

#define IO0PIN (*(volatile unsigned long *)0xE0028000)
/*****************************************************************************
** Function name:		EINT3_Handler
**
** Descriptions:		external INT handler
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void EINT3_Handler (void)
{
	EXTINT = EINT3;		/* clear interrupt */
	IO0_INT_CLR |= 15<<6;
	int d0, d1, d2, d3;
	d0 = (IO0PIN >>6)&1;
	d1 = (IO0PIN >>7)&1;
	d2 = (IO0PIN >>8)&1;
	d3 = (IO0PIN >>9)&1;
	if(!d0){
//	  	uart0Puts("d0\r\n");
	  	takeSnapshot=1;
	  	while(!d0)
	  		d0 = (IO0PIN >>6)&1;
	  	}
	if(!d1){
//		uart0Puts("d1\r\n");
		startRecording=1;
		while(!d1)
			d1 = (IO0PIN >>7)&1;
	}
	if(!d2){
		stopRecording=1;
//		uart0Puts("d2\r\n");
		while(!d2)
			d2 = (IO0PIN >>8)&1;
	}
/*	if(!d3){
		uart0Puts("d3\r\n");
		while(!d3)
			d3 = (IO0PIN >>9)&1;
	}*/
    disable_interrupt();
    VICVectAddr = 0;		/* Acknowledge Interrupt */
}

/*****************************************************************************
** Function name:		EINTInit
**
** Descriptions:		Initialize external interrupt pin and
**				install interrupt handler
**
** parameters:			None
** Returned value:		true or false, return false if the interrupt
**				handler can't be installed to the VIC table.
** 
*****************************************************************************/
#define IO0DIR (*(volatile unsigned long *)0xE0028008)
#define IO0PIN (*(volatile unsigned long *)0xE0028000)
DWORD EINTInit( void )
{
	PINSEL0 &= 0xFFF00FFF;
    IO0DIR &= 0xFFF00FFF;				/* input */
    SCS&=~1;
    EXTMODE = EINT3_EDGE;		/* INT1 edge trigger */
    EXTPOLAR = 0;			/* INT1 is falling edge by default */


    if ( install_irq( EINT3_INT, (void *)EINT3_Handler, MID_PRIORITY) == FALSE )
    {
	return (FALSE);
    }
    IO0_INT_EN_F  |= 15<<6;		/*set falling edge*/
    return( TRUE );
}

/******************************************************************************
**                            End Of File
******************************************************************************/

