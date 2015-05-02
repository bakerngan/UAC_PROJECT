/******************************************************************************
 * M O D I F I E D -------
 * LPC2119 Demo - stdio with newlib and analog to digital conversion
 * by Martin THOMAS <eversmith@heizung-thomas.de>
 *
 * Reads voltage at PINA0 in "Singe-Conversion/Burst-Mode"
 * and uses stdio/printf to view the result.
 *
 * ----------------------------------------------------------------------------
 *
 * - UART functions based on code from Bill Knight (R O Software)
 * - newlib stdio-interface from newlib-lpc (Aeolus Development)
 *   (may be easier to port to other targets in this form)
 * - V/V3A->V conversion based on an example found somewhere in the net
 *
 * ----------------------------------------------------------------------------
 *
 *  Tested with WinARM 4/05 (arm-elf-gcc 4.0.0) and 
 *  a Philips LPC2129 on a LPC-P2129 board (Olimex)
 *
 *  Review 20060710: 
 *  - updated makefile
 *  - test with WinARM 6/06 (arm-elf-gcc 4.1.1)
 *  Reviews 200603**
 *  - added fixes/improvements provided by Alexey Shusharin 
 *
 *****************************************************************************/

/*
	With arm-elf-gcc 4.0.0, newlib 1.13, Thumb-mode:
	section            size         addr
	.text             12939            0
	.data              2088   1073741824
	.bss                 68   1073743912
	.stack             1024   1073744128
	
	ARM-Mode:
	section            size         addr
	.text             18011            0
	
	With arm-elf-gcc 4.1.1, newlib 1.14, updated makefile
	             section            size         addr
	Thumb-mode:  .text             11208            0
	ARM-mode:    .text             15692            0
*/

#include "LPC24xx.h"

void systemInit(void)
{


	  if ( PLLSTAT & (1 << 25) )	// is PLL "connected"?
	  {
		PLLCON = 1;			// Enable PLL, disconnected
		PLLFEED = 0xaa;
		PLLFEED = 0x55;
	  }

	  PLLCON = 0;			// Disable PLL, disconnected
	  PLLFEED = 0xaa;
	  PLLFEED = 0x55;

//	  SCS |= 1;				//  Enable fast GPIO access 
//	  while( !(SCS & 0x40) );	// Wait until main OSC is usable

	  CLKSRCSEL = 0;		// pg. 46: select internal OSC, 4MHz, as the PLL clock source
	  //CLKSRCSEL = 0x1;		// pg. 46: select main OSC, 25MHz
	  //CLKSRCSEL = 0x2;		// pg. 46: select RTC OSC, 32kHz 

	  // pg. 49, 54: M and N bounds 
	  // for Main OSC, use M = 144 and N = 25 
	  // for RTC OSC, use M = 6750 and N = 1 
	  // for internal 4MHz osc, use M=144 and N=4 

	  //PLLCFG = 143 | (24 << 16);			// pg. 49: Main OSC PLL Setup
	  //PLLCFG = 6750 | (1 << 16);			// pg. 49: RTC OSC PLL Setup 
	  PLLCFG = (144-1) | ((4-1) << 16);		// pg. 49: Internal OSC PLL Setup 
	  PLLFEED = 0xaa;
	  PLLFEED = 0x55;

	  PLLCON = 1;					// pg. 49: Enable PLL, disconnected
	  PLLFEED = 0xaa;				// pg. 53
	  PLLFEED = 0x55;				// pg. 53

	  //CCLKCFG = CCLKDivValue;	/* Set clock divider */

	  CCLKCFG = (4-1);	/* pg. 57: Set clock divider (meant for Main OSC) */
	  //CCLKCFG = 5;	/* pg. 57: Set clock divider (meant for RTC OSC) */

	//#if USE_USB
	//  USBCLKCFG = USBCLKDivValue;		/* usbclk = 288 MHz/6 = 48 MHz */
	//#endif

	  while ( ((PLLSTAT & (1 << 26)) == 0) );	// wait for PLL to lock

//	  MValue = PLLSTAT & 0x00007FFF;		/* pg. 52: seems to just read in current M value */
//	  NValue = (PLLSTAT & 0x00FF0000) >> 16;		/* pg. 52: seems to just read in current N value */
	  //while ((MValue != 72) && ( NValue != 25) );		/* Main OSC */
	  //while ((MValue != 6750) && ( NValue != 1) );	/* RTC OSC */

	  PLLCON = 3;								// enable and connect PLL
	  PLLFEED = 0xaa;
	  PLLFEED = 0x55;
	  while ( ((PLLSTAT & (1 << 25)) == 0) );	// Check connect status
	  return;
	
}

static void delay(void )
{
	volatile int i,j;

	for (i=0;i<2000;i++)
		for (j=0;j<1000;j++);
}


