/*****************************************************************************
 *   spitest.c:  main C entry file for NXP LPC23xx/24xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.07.20  ver 1.00    Preliminary version, first Release
 *
******************************************************************************/
#include "LPC24xx.h"                        /* LPC23xx/24xx definitions */
#include "type.h"
#include "irq.h"
#include "spi.h"
#include "monitor.h"
#include "FAT.h"

BYTE SPICmd[BUFSIZE];
BYTE SPIWRData[BUFSIZE];
BYTE SPIRDData[BUFSIZE];
BYTE AckCounter = 0;

//#define print

extern BYTE recv[];

BYTE powerOff[8]={0xFF, 0xFF, 0xFF, 0x09, 0, 0, 0, 0};
BYTE sync[8]={0xFF, 0xFF, 0xFF, 0x0D, 0, 0, 0, 0};
BYTE ack[8]={0xFF, 0xFF, 0xFF, 0x0E, 0, 0, 0, 0};
BYTE nAck[8]={0xFF, 0xFF, 0xFF, 0x0F, 0, 0, 0, 0};
BYTE resetCmd[8]={0xFF, 0xFF, 0xFF, 0x08, 0, 0, 0, 0};
BYTE camInit[8]={0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x87, 0x07, 0x05};

//Colour Type: Compression Preview     Preview Resolution: 640x480   Compression Resolution: 640x480
BYTE picQuality[8]={0xFF, 0xFF, 0xFF, 0x10, 0, 0, 0, 0}; //set for best pic quality level
BYTE snapshot[8]={0xFF, 0xFF, 0xFF, 0x05, 0, 0, 0, 0};
BYTE getPicture[8] = {0xFF, 0xFF, 0xFF, 0x04, 0x05, 0, 0, 0};
BYTE resetcamera[8] = {0xFF, 0xFF, 0xFF, 0x08, 0x01, 0, 0, 0};
BYTE acksync[8]={0xFF, 0xFF, 0xFF, 0x0E, 0x0D, 0, 0, 0};
BYTE ackinit[8]={0xFF, 0xFF, 0xFF, 0x0E, 0x01, 0, 0, 0};
BYTE acksnap[8]={0xFF, 0xFF, 0xFF, 0x0E, 0x05, 0, 0, 0};
BYTE ackimgqual[8]={0xFF, 0xFF, 0xFF, 0x0E, 0x10, 0, 0, 0};
BYTE ackreset[8]={0xFF, 0xFF, 0xFF, 0x0E, 0x08, 0, 0, 0xFF};
int global=0;

char isAck(BYTE * resp, BYTE cmd)
{
	int i;
	for(i=0; i<8; i++)
	{
		switch(i)
		{
		case(3):if(resp[i]==0x0F)
		{
#ifdef print
			xprintf("\ra nak was returned!!!\n");
#endif
			return 0;}
		break;

		case(4): if(resp[i]!=cmd) return 0;
				 break;
		case(5): break;
		default: if(resp[i]!=ack[i]) return 0;
		}
	}
	return 1;
}


/*****************************************************************************
** Function name:		SPI0_Test
**
** Descriptions:		Test SPI0 as master interfacing with a Atmel
**						SPI based Serial EEPROM 25080
**
** parameters:			None
** Returned value:		None
**
*****************************************************************************/
/*
void SPI0_Test()
{
	while(1)
	{
		int i=0;
		SPICmd[0]=i;
		SPISend(SPICmd, 1);
		SPIReceive(SPIRDData, 1);
		i++;

	}
	}
*/

void sendSync()
	{
    int i = 0;
    int j=0;

	IODIR0 = (IODIR0&~0x01800000)|0x01000000;
	IOPIN0 &= ~0x1000000;		// drive CS low

    //sync loop
	while(i++!=60)
		{
#ifdef print
			xprintf("SYNC: (try #%d)   ", i);
#endif
			//if the OV529 is not ready, loop
			while((volatile int)IOPIN0 & (1<<23)) ;

			SPISend(sync, 8);

			//for(j=0; j<100000; j++);	// delay approx 25msec

			SPIReceive(SPIRDData, 8);
#ifdef print
			xprintf("MISO: %02x %02x %02x %02x %02x %02x %02x %02x\r\n ",
					SPIRDData[0], SPIRDData[1], SPIRDData[2], SPIRDData[3],
					SPIRDData[4], SPIRDData[5], SPIRDData[6], SPIRDData[7]);
#endif
			if(isAck(SPIRDData, 0x0D))
				{
#ifdef print
					xprintf("ack sync was received\n");
#endif
					SPIReceive(SPIRDData, 8);		// receive SYNC cmd from camera
					for(j=0; j<8; j++) {
						if(SPIRDData[j] != sync[j]) {
#ifdef print
							xprintf("Expected SYNC, got: %02x %02x %02x %02x %02x %02x %02x %02x\r\n ",
									SPIRDData[0], SPIRDData[1], SPIRDData[2], SPIRDData[3],
									SPIRDData[4], SPIRDData[5], SPIRDData[6], SPIRDData[7]);
#endif
							// don't exactly know what should be done in this case
							// for the moment, let's send ACK and try to continue ....
						}
					}
#ifdef print
					if(j==8) xprintf("Got SYNC from camera\n");
#endif
					SPISend(acksync, 8);
					AckCounter++;
					acksync[5] = AckCounter;
					break;
				}
		}
	IOPIN0 |= 0x1000000;	// drive CS high
	}

void resetCam()
	{
    int i = 0;
    int j = 0;
	IODIR0 = (IODIR0&~0x01800000)|0x01000000;
	IOPIN0 &= ~0x1000000;		// drive CS low

    //reset loop
    while(i++ != 10)
    {
#ifdef print
		xprintf("RESET: (try #%d)   ",i);
#endif
		//if the OV529 is not ready, loop
		while((volatile int)IOPIN0 & (1<<23)) ;

		SPISend(resetcamera, 8);
		//for(j=0; j<100000; j++);
		SPIReceive(SPIRDData, 8);
#ifdef print
		xprintf("MISO: %02x %02x %02x %02x %02x %02x %02x %02x\r\n ",
				SPIRDData[0], SPIRDData[1], SPIRDData[2], SPIRDData[3],
				SPIRDData[4], SPIRDData[5], SPIRDData[6], SPIRDData[7]);
#endif
		if(isAck(SPIRDData, 0x08))
			{
#ifdef print
				xprintf("ack reset was received\n");
#endif
				break;
			}
    }
	IOPIN0 |= 0x1000000;	// drive CS high
}

void getInit()
	{
    int j=0;
    int i=0;
    IODIR0 = (IODIR0&~0x01800000)|0x01000000;
    IOPIN0 &= ~0x1000000;		// drive CS low

while(i++!=15)
{
#ifdef print
	xprintf("INIT:   " );
#endif
	//if the OV529 is not ready, loop
	while((volatile int)IOPIN0 & (1<<23)) ;

	SPISend(camInit, 8);
	//for(j=0; j<100000; j++);
	SPIReceive(SPIRDData, 8);
#ifdef print
	xprintf("MISO: %02x %02x %02x %02x %02x %02x %02x %02x\r\n ",
			SPIRDData[0], SPIRDData[1], SPIRDData[2], SPIRDData[3],
			SPIRDData[4], SPIRDData[5], SPIRDData[6], SPIRDData[7]);
#endif
	if(isAck(SPIRDData, 0x01))
		{
#ifdef print
			xprintf("ack camera init was received\n");
#endif
			break;
		}
}
	IOPIN0 |= 0x1000000;	// drive CS high
}

void getImageQuality()
	{
    int j = 0;
    int i=0;
    IODIR0 = (IODIR0&~0x01800000)|0x01000000;
    IOPIN0 &= ~0x1000000;		// drive CS low
#ifdef print
	xprintf("IMAGE QUALITY:   ");
#endif
	//if the OV529 is not ready, loop
	while((volatile int)IOPIN0 & (1<<23)) ;

	SPISend(picQuality, 8);
	//for(j=0; j<100000; j++);

	SPIReceive(SPIRDData, 8);
#ifdef print
	xprintf("MISO: %02x %02x %02x %02x %02x %02x %02x %02x\r\n ",
			SPIRDData[0], SPIRDData[1], SPIRDData[2], SPIRDData[3],
			SPIRDData[4], SPIRDData[5], SPIRDData[6], SPIRDData[7]);
#endif
	if(isAck(SPIRDData, 0x10))
		{
#ifdef print
			xprintf("ack image quality was received\n");
#endif
			SPISend(ackimgqual, 8);

		}

	IOPIN0 |= 0x1000000;	// drive CS high
}

void getSnapshot()
	{
    int i = 0;
    int j=0;

	IODIR0 = (IODIR0&~0x01800000)|0x01000000;
	IOPIN0 &= ~0x1000000;	// drive CS low
while(i++!=15)
{
#ifdef print
	xprintf("GET SNAPSHOT: (try #%d)   ", i);
#endif
	//if the OV529 is not ready, loop
	while((volatile int)IOPIN0 & (1<<23)) ;

	SPISend(snapshot, 8);
	//for(j=0; j<100000; j++);

	SPIReceive(SPIRDData, 8);
#ifdef print
	xprintf("MISO: %02x %02x %02x %02x %02x %02x %02x %02x\r\n ",
			SPIRDData[0], SPIRDData[1], SPIRDData[2], SPIRDData[3],
			SPIRDData[4], SPIRDData[5], SPIRDData[6], SPIRDData[7]);
#endif
	if(isAck(SPIRDData, 0x05))
		{
#ifdef print
			xprintf("ack snapshot was received\n");
#endif
			break;
		}
}
	IOPIN0 |= 0x1000000;	// drive CS high
}

int getPic()  //return image size
{
    int i = 0;
    int j = 0;
    int imgsz;
    int size;
//    BYTE *imagebuf = (BYTE*)0x80000000;	// SRAM area starting address (used for image buffer)
    BYTE *imagebuf = (BYTE*)0x40004000;	// ON-CHIP SRAM area starting address (used for image buffer)

	IODIR0 = (IODIR0&~0x01800000)|0x01000000;
	IOPIN0 &= ~0x1000000;	// drive CS low

	    while(i++ != 30)
	    {
#ifdef print
			xprintf("GET PICTURE: (try #%d) \n  ", i);
#endif
			//if the OV529 is not ready, loop
			while((volatile int)IOPIN0 & (1<<23)) ;

			SPISend(getPicture, 8);
			//for(j=0; j<100000; j++);

			SPIReceive(SPIRDData, 8);
#ifdef print
			xprintf("MISO: %02x %02x %02x %02x %02x %02x %02x %02x\r\n ",
					SPIRDData[0], SPIRDData[1], SPIRDData[2], SPIRDData[3],
					SPIRDData[4], SPIRDData[5], SPIRDData[6], SPIRDData[7]);
#endif
			if(isAck(SPIRDData, 0x04))
				{
#ifdef print
					xprintf("ack getPic was received\n");
#endif
					SPIReceive(SPIRDData, 8);
#ifdef print
					xprintf("MISO: %02x %02x %02x %02x %02x %02x %02x %02x\r\n ",
							SPIRDData[0], SPIRDData[1], SPIRDData[2], SPIRDData[3],
							SPIRDData[4], SPIRDData[5], SPIRDData[6], SPIRDData[7]);
#endif
					for(j=0; j<4; j++) {
						if((j!=3 && SPIRDData[j] != 0xff) ||
						   (j==3 && SPIRDData[j] != 0x0A) ) {
#ifdef print
							xprintf("Expected FFFFFF0A... \n");
#endif
							break;
						}
					}
					if(j==4) {
#ifdef print
						xprintf("Data size: %d\r\n",
								imgsz = (SPIRDData[5] + (SPIRDData[6]<<8) + (SPIRDData[7]<<16)));
#endif
						imgsz = (SPIRDData[5] + (SPIRDData[6]<<8) + (SPIRDData[7]<<16));
						        size=imgsz;
						/*int index = 0;                // position where next 8 byte chunk goes in overall image
						while (imgsz> 0) {
						    SPIReceive(&imagebuf[index], 8); // read in next 8 bytes (including driving CS low, reading, then driving it high)
						    imgsz -= 8;
						    index += 8;
						}*/


						SPIReceive(imagebuf, imgsz);
						break;
					}
				}
	    }
		IOPIN0 |= 0x1000000;	// drive CS high
		return size;
}

void stopRec(){
	global=1;
}
void setupCamera()
{
	sendSync();
	getInit();
	getImageQuality();
}
int takePicture()
{
	getSnapshot();
	return getPic();
}

void record()
{
	while(global!=1)
	{
		getSnapshot();
		writeToSDCard(getPic());
	}
	global=0;
	closeFile();
}



///*****************************************************************************
// *   spitest.c:  main C entry file for NXP LPC23xx/24xx Family Microprocessors
// *
// *   Copyright(C) 2006, NXP Semiconductor
// *   All rights reserved.
// *
// *   History
// *   2006.07.20  ver 1.00    Preliminary version, first Release
// *
//******************************************************************************/
//#include "LPC24xx.h"                        /* LPC23xx/24xx definitions */
//#include "type.h"
//#include "irq.h"
//#include "spi.h"
//#include "monitor.h"
//#include "FAT.h"
//
//BYTE SPICmd[BUFSIZE];
//BYTE SPIWRData[BUFSIZE];
//BYTE SPIRDData[BUFSIZE];
//BYTE AckCounter = 0;
//
//#define cameraPrint     //Comment out this to use print
//
//extern BYTE recv[];
//
//BYTE powerOff[8]={0xFF, 0xFF, 0xFF, 0x09, 0, 0, 0, 0};
//BYTE sync[8]={0xFF, 0xFF, 0xFF, 0x0D, 0, 0, 0, 0};
//BYTE ack[8]={0xFF, 0xFF, 0xFF, 0x0E, 0, 0, 0, 0};
//BYTE nAck[8]={0xFF, 0xFF, 0xFF, 0x0F, 0, 0, 0, 0};
//BYTE resetCmd[8]={0xFF, 0xFF, 0xFF, 0x08, 0, 0, 0, 0};
//BYTE camInit[8]={0xFF, 0xFF, 0xFF, 0x01, 0, 0x87, 0x07, 0x07};
//
////BYTE camInit[8]={0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x05, 0x05, 0x07};
//
//BYTE pkg_size[8] = {0xFF, 0xFF, 0xFF, 0x06, 0x08, 0x00, 0x00, 0x00};	// 512 byte pkgs
//BYTE ack_pkg[8]  = {0xFF, 0xFF, 0xFF, 0x0E, 0x00, 0x00, 0x00, 0x00};
//
///*Colour Type: Compression Preview     Preview Resolution: 640x480   Compression Resolution: 640x480*/
//BYTE picQuality[8]={0xFF, 0xFF, 0xFF, 0x10, 0, 0, 0, 0}; /*set for best pic quality level*/
//BYTE snapshot[8]={0xFF, 0xFF, 0xFF, 0x05, 0, 0, 0, 0};
//BYTE getPicture[8] = {0xFF, 0xFF, 0xFF, 0x04, 0x05, 0, 0, 0};
//BYTE resetcamera[8] = {0xFF, 0xFF, 0xFF, 0x08, 0x01, 0, 0, 0};
//BYTE acksync[8]={0xFF, 0xFF, 0xFF, 0x0E, 0x0D, 0, 0, 0};
//BYTE ackinit[8]={0xFF, 0xFF, 0xFF, 0x0E, 0x01, 0, 0, 0};
//BYTE acksnap[8]={0xFF, 0xFF, 0xFF, 0x0E, 0x05, 0, 0, 0};
//BYTE ackimgqual[8]={0xFF, 0xFF, 0xFF, 0x0E, 0x10, 0, 0, 0};
//BYTE ackreset[8]={0xFF, 0xFF, 0xFF, 0x0E, 0x08, 0, 0, 0xFF};
//
//
//char isAck(BYTE * resp, BYTE cmd)
//{
//	int i;
//	for(i=0; i<8; i++)
//	{
//		switch(i)
//		{
//		case(3):if(resp[i]==0x0F)
//		{
//#ifdef cameraPrint
//			xprintf("\ra NAK, error code = %d\n",resp[7]);
//#endif
//			return 0;}
//		break;
//
//		case(4): if(resp[i]!=cmd) return 0;
//				 break;
//		case(5): break;
//		default: if(resp[i]!=ack[i]) return 0;
//		}
//	}
//	return 1;
//}
//
//
///*****************************************************************************
//** Function name:		SPI0_Test
//**
//** Descriptions:		Test SPI0 as master interfacing with a Atmel
//**						SPI based Serial EEPROM 25080
//**
//** parameters:			None
//** Returned value:		None
//**
//*****************************************************************************/
///*
//void SPI0_Test()
//{
//	while(1)
//	{
//		int i=0;
//		SPICmd[0]=i;
//		SPISend(SPICmd, 1);
//		SPIReceive(SPIRDData, 1);
//		i++;
//
//	}
//	}
//*/
//
//void sendSync()
//	{
//    int i = 0;
//    int j=0;
//
//	//IODIR0 = (IODIR0&~0x01800000)|0x01000000;
//	//IOPIN0 &= ~0x1000000;		// drive CS low
//
//    /*sync loop*/
//	while(i++!=60)
//		{
//#ifdef cameraPrint
//			xprintf("SYNC: (try #%d)   ", i);
//#endif
//
//			//if the OV529 is not ready, loop
//			while((volatile int)IOPIN0 & (1<<23)) ;
//
//			SPISend(sync, 8);
//
//			for(j=0; j<100000; j++);	// delay approx 25msec
//
//			SPIReceive(SPIRDData, 8);
//#ifdef cameraPrint
//			xprintf("MISO: %02x %02x %02x %02x %02x %02x %02x %02x\r\n ",
//					SPIRDData[0], SPIRDData[1], SPIRDData[2], SPIRDData[3],
//					SPIRDData[4], SPIRDData[5], SPIRDData[6], SPIRDData[7]);
//#endif
//			if(isAck(SPIRDData, 0x0D))
//				{
//#ifdef cameraPrint
//					xprintf("ack sync was received\n");
//#endif
//					SPIReceive(SPIRDData, 8);		// receive SYNC cmd from camera
//					for(j=0; j<8; j++) {
//						if(SPIRDData[j] != sync[j]) {
//#ifdef cameraPrint
//							xprintf("Expected SYNC, got: %02x %02x %02x %02x %02x %02x %02x %02x\r\n ",
//									SPIRDData[0], SPIRDData[1], SPIRDData[2], SPIRDData[3],
//									SPIRDData[4], SPIRDData[5], SPIRDData[6], SPIRDData[7]);
//#endif
//							// don't exactly know what should be done in this case
//							// for the moment, let's send ACK and try to continue ....
//						}
//					}
//#ifdef cameraPrint
//					if(j==8) xprintf("Got SYNC from camera\n");
//#endif
//					SPISend(acksync, 8);
//					AckCounter++;
//					acksync[5] = AckCounter;
//					break;
//				}
//		}
//	//IOPIN0 |= 0x1000000;	// drive CS high
//}
//
//void resetCam()
//	{
//    int i = 0;
//    int j = 0;
//	//IODIR0 = (IODIR0&~0x01800000)|0x01000000;
//	//IOPIN0 &= ~0x1000000;		// drive CS low
//
//    //reset loop
//    while(i++ != 10)
//    {
//#ifdef cameraPrint
//		xprintf("RESET: (try #%d)   ",i);
//#endif
//		//if the OV529 is not ready, loop
//		while((volatile int)IOPIN0 & (1<<23)) ;
//
//		SPISend(resetcamera, 8);
//		//for(j=0; j<100000; j++);
//		SPIReceive(SPIRDData, 8);
//#ifdef cameraPrint
//		xprintf("MISO: %02x %02x %02x %02x %02x %02x %02x %02x\r\n ",
//				SPIRDData[0], SPIRDData[1], SPIRDData[2], SPIRDData[3],
//				SPIRDData[4], SPIRDData[5], SPIRDData[6], SPIRDData[7]);
//#endif
//		if(isAck(SPIRDData, 0x08))
//			{
//#ifdef cameraPrint
//				xprintf("ack reset was received\n");
//#endif
//				break;
//			}
//    }
//	//IOPIN0 |= 0x1000000;	// drive CS high
//}
//
//void getInit()
//	{
//    int j=0;
//    int i=0;
//  //  IODIR0 = (IODIR0&~0x01800000)|0x01000000;
//   // IOPIN0 &= ~0x1000000;		// drive CS low
//
//while(i++!=15)
//{
//#ifdef cameraPrint
//	xprintf("INIT:   " );
//#endif
//
//	//if the OV529 is not ready, loop
//	while((volatile int)IOPIN0 & (1<<23)) ;
//
//	SPISend(camInit, 8);
//	//for(j=0; j<100000; j++);
//	SPIReceive(SPIRDData, 8);
//#ifdef cameraPrint
//	xprintf("MISO: %02x %02x %02x %02x %02x %02x %02x %02x\r\n ",
//			SPIRDData[0], SPIRDData[1], SPIRDData[2], SPIRDData[3],
//			SPIRDData[4], SPIRDData[5], SPIRDData[6], SPIRDData[7]);
//#endif
//	if(isAck(SPIRDData, 0x01))
//		{
//#ifdef cameraPrint
//			xprintf("ack camera init was received\n");
//#endif
//			break;
//		}
//}
//	//IOPIN0 |= 0x1000000;	// drive CS high
//}
//
//void getImageQuality()
//	{
//    int j = 0;
//    int i=0;
//   // IODIR0 = (IODIR0&~0x01800000)|0x01000000;
//   // IOPIN0 &= ~0x1000000;		// drive CS low
//#ifdef cameraPrint
//	xprintf("IMAGE QUALITY:   ");
//#endif
//	//if the OV529 is not ready, loop
//	while((volatile int)IOPIN0 & (1<<23)) ;
//
//	SPISend(picQuality, 8);
//	//for(j=0; j<100000; j++);
//
//	SPIReceive(SPIRDData, 8);
//#ifdef cameraPrint
//	xprintf("MISO: %02x %02x %02x %02x %02x %02x %02x %02x\r\n ",
//			SPIRDData[0], SPIRDData[1], SPIRDData[2], SPIRDData[3],
//			SPIRDData[4], SPIRDData[5], SPIRDData[6], SPIRDData[7]);
//#endif
//	if(isAck(SPIRDData, 0x10))
//		{
//#ifdef cameraPrint
//			xprintf("ack image quality was received\n");
//#endif
//			SPISend(ackimgqual, 8);
//		}
//
//	//IOPIN0 |= 0x1000000;	// drive CS high
//}
//
//void getSnapshot()
//	{
//    int i = 0;
//    int j=0;
//
//	//IODIR0 = (IODIR0&~0x01800000)|0x01000000;
//	//IOPIN0 &= ~0x1000000;	// drive CS low
//while(i++!=15)
//{
//#ifdef cameraPrint
//	xprintf("GET SNAPSHOT: (try #%d)   ", i);
//#endif
//
//	//if the OV529 is not ready, loop
//	while((volatile int)IOPIN0 & (1<<23)) ;
//
//	SPISend(snapshot, 8);
//
////	for(j=0; j<100000; j++);
//
//	SPIReceive(SPIRDData, 8);
//#ifdef cameraPrint
//	xprintf("MISO: %02x %02x %02x %02x %02x %02x %02x %02x\r\n ",
//			SPIRDData[0], SPIRDData[1], SPIRDData[2], SPIRDData[3],
//			SPIRDData[4], SPIRDData[5], SPIRDData[6], SPIRDData[7]);
//#endif
//	if(isAck(SPIRDData, 0x05))
//		{
//#ifdef cameraPrint
//			xprintf("ack snapshot was received\n");
//#endif
//			break;
//		}
//}
//	//IOPIN0 |= 0x1000000;	// drive CS high
//}
//
//int getPic()  //return image size
//{
//    int i = 0;
//    int j = 0;
//    int imgsz;
//    int size;
//    BYTE *imagebuf = (BYTE*)0x80000000;	// SRAM area starting address (used for image buffer)
//    //BYTE *imagebuf = (BYTE*)0x40004000;	// ON-CHIP SRAM area starting address (used for image buffer)
//
//	//IODIR0 = (IODIR0&~0x01800000)|0x01000000;
//	//IOPIN0 &= ~0x1000000;	// drive CS low
//
//	    while(i++ != 30)
//	    {
//#ifdef cameraPrint
//			xprintf("GET PICTURE: (try #%d) \n  ", i);
//#endif
//
//			//if the OV529 is not ready, loop
//			while((volatile int)IOPIN0 & (1<<23)) ;
//
//			SPISend(getPicture, 8);
////			for(j=0; j<100000; j++);
//
//			SPIReceive(SPIRDData, 8);
//#ifdef cameraPrint
//			xprintf("MISO: %02x %02x %02x %02x %02x %02x %02x %02x\r\n ",
//					SPIRDData[0], SPIRDData[1], SPIRDData[2], SPIRDData[3],
//					SPIRDData[4], SPIRDData[5], SPIRDData[6], SPIRDData[7]);
//#endif
//			if(isAck(SPIRDData, 0x04))
//				{
//#ifdef cameraPrint
//					xprintf("ack getPic was received\n");
//
//#endif
//				//	if(isAck(SPIRDData, 0x0A)){
//					SPIReceive(SPIRDData, 8);
//#ifdef cameraPrint
//					xprintf("MISO: %02x %02x %02x %02x %02x %02x %02x %02x\r\n ",
//							SPIRDData[0], SPIRDData[1], SPIRDData[2], SPIRDData[3],
//							SPIRDData[4], SPIRDData[5], SPIRDData[6], SPIRDData[7]);
//#endif
//					for(j=0; j<4; j++) {
//						if((j!=3 && SPIRDData[j] != 0xff) ||
//						   (j==3 && SPIRDData[j] != 0x0A) ) {
//#ifdef cameraPrint
//							xprintf("Expected FFFFFF0A... \n");
//#endif
//							break;
//						}
//					}
//					while((volatile int)IOPIN0 & (1<<23)) ;
//					if(j==4) {
//						imgsz = (SPIRDData[5] + (SPIRDData[6]<<8) + (SPIRDData[7]<<16));
//						size=imgsz;
//#ifdef cameraPrint
//						xprintf("Data size: %d Data Type: %d\r\n",imgsz,SPIRDData[4]);
//#endif
//#if 0
//						int index = 0;        // position where next 8 byte chunk goes in overall image
//						while(imgsz > 0) {
//							int m;
//							// SPIReceiveByte();
//							for(m=0; m<64; m++) {
//								SPIReceive(&imagebuf[index], 8); // read in next 8 bytes (including driving CS low, reading, then driving it high)
//								index += 8;
//							}
//							imgsz -= 512;
//						}
//
//#else
//						//SPIReceive(imagebuf, imgsz);
//						readImage(imagebuf, imgsz);
//#endif
//						break;
//					}
//				}
//
//	    }
//		//IOPIN0 |= 0x1000000;	// drive CS high
//		return size;
//}
//void setupCamera()
//{
//	sendSync();
//	//getInit();
//	//sendSync();
//	getInit();
//	getImageQuality();
//	//setPkgSize(512);
//}
//
//void setPkgSize( int size )
//{
//    int j=0;
// 	IODIR0 = (IODIR0&~0x01800000)|0x01000000;
//	IOPIN0 &= ~0x1000000;		// drive CS low
//
//	xprintf("SET PKG SIZE:   " );
//
//	while((volatile int)IOPIN0 & (1<<23)) ;	  // loop while OV529 not ready
//
//	SPISend(pkg_size, 8);
//
//	SPIReceive(SPIRDData, 8);
//
//	xprintf("MISO: %02x %02x %02x %02x %02x %02x %02x %02x\r\n ",
//		SPIRDData[0], SPIRDData[1], SPIRDData[2], SPIRDData[3],
//			SPIRDData[4], SPIRDData[5], SPIRDData[6], SPIRDData[7]);
//
//	if(isAck(SPIRDData, 0x06)) xprintf("ack set pkg size was received\n");
//
//	IOPIN0 |= 0x1000000;	// drive CS high
//}
//
//// Routine to receive image of "isize" bytes using packages
//// Though datasheet omits any mentions of packages, we have
//// learned by studying other cameras based on the OV529 chip
//// (specifically the 4D uCAM529-SPI) that this is necessary.
//// One must use the SET PACKAGE SIZE command (before sending
//// any GET SNAPSHOT or GET PICTURE commands) to change the
//// default (64 bytes) to a larger, more efficient size.  In
//// our case, we are using 512 bytes, which is teh maximum allowed.
//
//void readImage( BYTE *buf, int isize )
//{
//	int psize;				// size of next package
//	int index = 0;			// index into data buffer area
//	int sum;
//	int k;
//	BYTE pck_id = 0;
//	BYTE hdr[4];
//	BYTE tmp;
//	BYTE nakflag;
//
//	ack_pkg[6] = 0;             // low byte of initial package ID
//	ack_pkg[7] = 0;             // high byte of initial package ID
//	SPISend(ack_pkg, 8);		// send ACK to request first package
//
//	while(isize > 0) {
//		psize = (isize > 512) ? 512 : isize;	// choose package size
//
//		// Receive the package: pck_id(low) pkg_id(hi)
//		//						size(low)  size(hi)
//		//						<image data> chksum(2 bytes)
//		SPIReceive(hdr, 4);
//		tmp =  hdr[2]+(hdr[3]<<8);	// reassemble data size field as integer
//		xprintf("Received pkg ID %d (sz=%d)\r\n",hdr[0]+(hdr[1]<<8),tmp);
//
//		nakflag = 0;				// initialize (expecting success)
//		if(tmp != psize-6) {
//			xprintf("Wrong package data size.  Expected %d, got %d\r\n",
//					psize-6, tmp);
//			nakflag = 1;
//		}
//		SPIReceive(&buf[index], tmp+2);		// get the image data + chksum
//		for(k=4, sum=0; k<tmp-2; k++) sum += buf[index+k];
//		if((sum&0xff) != buf[index+tmp-1]) {
//			xprintf("Data package checksum mismatch.  Expected %d, got %d\r\n",
//					sum&0xff, buf[index+tmp-1]);
//			nakflag = 1;
//		}
//
//		if(nakflag == 0) {
//			index = index+tmp;	// adjust index for next time
//			pck_id = pck_id+1;	// next pkg ID
//			isize -= tmp;		// decrement count of data so far
//		}
//		pck_id = (isize) ? pck_id : 0xf0f0;	// if done, special pkg ID
//		ack_pkg[6] = pck_id & 0x1ff;	// low byte of package ID
//		ack_pkg[7] = pck_id >> 8;		// high byte of package ID
//		SPISend(ack_pkg, 8);			// send ACK to trigger next pkg
//	}
//	return;
//}

