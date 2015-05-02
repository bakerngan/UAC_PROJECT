/*---------------------------------------------------------------*/
/* FAT file system module test program R0.05   (C)ChaN, 2007     */
/* Extended for LPC23xx/24xx MCI Test          (C)Martin Thomas  */
/*---------------------------------------------------------------*/

#include "LPC24xx.h"
#include "irq.h"
#include "monitor.h"
#include "sysinit.h"

#include "uart.h"		//Uart interface
#include "spi.h"		//SPI Camera
#include "extint.h"		//external interrupt
#include "FAT.h"		//SDCard

//This is for the UART
#define IO0DIR (*(volatile unsigned long *)0xE0028008)
#define IO0PIN (*(volatile unsigned long *)0xE0028000)
//Global flags that are set by the external interrupt
int takeSnapshot=0;
int startRecording=0;
int stopRecording=0;

void SRAMInit(){
	  PINSEL6 = 0x55555555;
	  PINSEL8 = PINSEL8 & 0x00000000;
	  PINSEL8 = PINSEL8 | 0x55555555; 	  //Set all 32:0 pins to 01 for address P4[0] - P4[15]

	  PINSEL9 = PINSEL9 & 0x0F00FF00;	  //Set 7:0 pins to 01 for address P4[16] - P4[19]
	  PINSEL9 = PINSEL9 | 0x50550055;

	  EMC_CTRL &= ~2;					// select normal memory map (enables CS0 operation)
	  EMC_STA_CFG0 = 0x81;
	  EMC_STA_WAITWEN0 = 0x0;			// Keil had 2
	  EMC_STA_WAITOEN0 = 0x0;			//          3
	  EMC_STA_WAITRD0 = 0x1;			//          2
	  EMC_STA_WAITPAGE0 = 0x2;
	  EMC_STA_WAITWR0 = 0x0;			//          2
	  EMC_STA_WAITTURN0 = 0x2;
}


static
void IoInit (void)
{
	systemInit();
	uart0Init(B115200 ,UART_8N1 , UART_FIFO_8);
	SPIInit();		/* initialize SPI 0 */
	setupCamera();
	init_VIC();
	EINTInit();
	SRAMInit();
	enable_interrupt();
	initSDCard();
}

int main (void)
{
	IoInit();
	xprintf("HelloWorld\r\n");
	int size=10000;
	int i=0;
	while(1)
	{
		if(takeSnapshot)
		{
			xprintf("\ntake snapshot\r");
			SDOpenSnapshot();
			getSnapshot();
			size=getPic();
			xprintf("\nsize: %d\r",size);
			writeToSDCard(size);
			closeFile();
			takeSnapshot=0;
			xprintf("\nend\r\n");
		}
		if(startRecording)
		{
			xprintf("\nstartRecording\r");
			SDOpenVideo();
			while(stopRecording!=1)
			{
				getSnapshot();
				size=getPic();
				writeToSDCard(size);
				i++;
			}
			closeFile();
			startRecording=0;
			stopRecording=0;
			xprintf("\nimages stored=%d\r\n",i);
		}
		if(startRecording==0 && stopRecording)
			stopRecording=0;
	}
}
