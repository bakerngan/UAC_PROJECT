/*****************************************************************************
 *   mci.h:  Header file for NXP LPC23xx/24xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.07.20  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/

/* Modified and extended by Martin Thomas <mthomas(at)rhkr(dot)uni-kl(dot)de> */

#ifndef __MCI_H
#define __MCI_H

#define MCI_DMA_ENABLED		1
// #define MCI_DMA_ENABLED		0

/* For the earliest MCB2300 board, set below bit to 1, for the newer
one, set it to zero. */
#define MCB2300_VERSION_0	0

/* see master definition file lpc230x.h for more details */
#define MCI_FIFO_BASE		MCI_BASE_ADDR + 0x80
#define MCI_FIFO_INDEX		0x04

/* SD/MMC Command list, per MMC spec. SD Memory Card Spec. Simplified version */
#define GO_IDLE_STATE		0		/* GO_IDLE_STATE(MMC) or RESET(SD) */
#define SEND_OP_COND		1		/* SEND_OP_COND(MMC) or ACMD41(SD) */
#define ALL_SEND_CID		2		/* ALL SEND_CID */
#define SET_RELATIVE_ADDR	3		/* SET_RELATE_ADDR */
#define SET_ACMD_BUS_WIDTH	6
#define SELECT_CARD			7		/* SELECT/DESELECT_CARD */
#define SEND_CSD			9		/* SEND_CSD */
#define STOP_TRANSMISSION	12		/* Stop either READ or WRITE operation */
#define SEND_STATUS			13		/* SEND_STATUS */
#define SET_BLOCK_LEN		16		/* SET_BLOCK_LEN */
#define READ_SINGLE_BLOCK	17		/* READ_SINGLE_BLOCK */
#define WRITE_BLOCK			24		/* WRITE_BLOCK */
#define WRITE_MULTIPLE_BLOCK 25		/* WRITE_MULTIPLE_BLOCK */ 
#define SEND_APP_OP_COND	41		/* ACMD41 for SD card */
#define APP_CMD				55		/* APP_CMD, the following will a ACMD */
#define SD_STATUS			13		/* ACMD13 for SD card (mthomas) */

#define OCR_INDEX			0x00FF8000
// #define OCR_INDEX			0x00000000
// #define OCR_INDEX			1 << 23 

/* added by Martin Thomas - based on information from
   Juri Haberland */
#define SEND_IF_COND       8          /* mthomas: added */
#define SEND_IF_COND_ARG   0x000001AA /* mthomas: added */
/* OCR register bit definitions */
#define OCR_LOW_VOLTAGE_RANGE   0x00000080      /* low voltage range: 1.65 - 1.95V */ 
#define OCR_HIGH_VOLTAGE_RANGE  0x00FF8000      /* high voltage range: 2.7 - 3.6V */
#define OCR_CARD_HIGH_CAPACITY  0x40000000      /* Card high capacity status bit */
#define OCR_CARD_NOT_BUSY       0x80000000      /* Card Power up status bit */
// mthomas end


#define CARD_STATUS_ACMD_ENABLE		1 << 5
#define CARD_STATUS_RDY_DATA		1 << 8
#define CARD_STATUS_CURRENT_STATE	0x0F << 9
#define CARD_STATUS_ERASE_RESET		1 << 13

#define SLOW_RATE			1
#define NORMAL_RATE			2

#define SD_1_BIT 			0
#define SD_4_BIT			1

#if 1
// modified by Martin Thomas
#define CARD_UNKNOWN     0
#define MMC_CARD         (1<<0)
#define SDSC_CARD        (1<<1)
#define SD_CARD          (1<<1) /* alias for SDSC_CARD NXP-compatibilty*/
#define SDHC_CARD        (1<<2)
#define SDV2_CARD        (1<<3) /* only used temporarly */
#else
// NXP original:
#define CARD_UNKNOWN		0
#define MMC_CARD			1
#define SD_CARD				2
#endif

/* MCI clk freq = Pclk/(2* (Clkdiv +1) */
/* change 20080716 - hex ist wrong should be dec, thanks to Heiko Will */
// old and wrong: #define MCLKDIV_SLOW  0x60-1
/* also changed prescaler to 60 so freq is < 400kHz as recommended in the
   user-manual */
#define MCLKDIV_SLOW            (62-1)   /* 61: 393,442kHz = @48Mhz/(2*(61+1)) < 400kHz */ 
// #define MCLKDIV_NORMAL       (0x4-1)  /* 3 = 6Mhz -> @48Mhz/(2*4) */
#define MCLKDIV_NORMAL          0  /* 1 = 12Mhz -> @48Mhz/(2*2) */

// mt #define DATA_TIMER_VALUE	0x10000
#define DATA_TIMER_VALUE	(0x10000*2)

#define EXPECT_NO_RESP		0
#define EXPECT_SHORT_RESP	1
#define EXPECT_LONG_RESP	2

/* For the SD card I tested, the minimum block length is 512 */
/* For MMC, the restriction is loose, due to the variety of SD and MMC
card support, ideally, the driver should read CSD register to find the 
speed and block length for the card, and set them accordingly. */
/* In this driver example, it will support both MMC and SD cards, it
does read the information by send SEND_CSD to poll the card status,
but, it doesn't configure them accordingly. this is not intended to 
support all the SD and MMC card. */ 

/* DATA_BLOCK_LEN table
	DATA_BLOCK_LEN			Actual Size( BLOCK_LENGTH )
	11						2048
	10						1024
	9						512
	8						256
	7						128
	6						64
	5						32
	4						16
	3						8
	2						4
	1						2
*/

/* To simplify the programming, please note that, BLOCK_LENGTH is a multiple 
of FIFO_SIZE */
#define DATA_BLOCK_LEN		9	/* Block size field in DATA_CTRL */
#define BLOCK_LENGTH		(1 << DATA_BLOCK_LEN)
								/* for SD card, 128, the size of the flash */
								/* card is 512 * 128 = 64K */

// mthomas:
#define SD_STATUS_BLOCK_LEN	6
#define SD_BLOCK_LENGTH     ( 1 << SD_STATUS_BLOCK_LEN)

// mthomas - reduced number of test-blocks #define BLOCK_NUM			0x80
#define BLOCK_NUM			0x10
#define FIFO_SIZE			16


#define BUS_WIDTH_1BIT		0
#define BUS_WIDTH_4BITS		10

/* MCI Status register bit information */
#define MCI_CMD_CRC_FAIL	(1 << 0)
#define MCI_DATA_CRC_FAIL	(1 << 1)
#define MCI_CMD_TIMEOUT		(1 << 2)
#define MCI_DATA_TIMEOUT	(1 << 3)
#define MCI_TX_UNDERRUN		(1 << 4)
#define MCI_RX_OVERRUN		(1 << 5)
#define MCI_CMD_RESP_END	(1 << 6)
#define MCI_CMD_SENT		(1 << 7)
#define MCI_DATA_END		(1 << 8)
#define MCI_START_BIT_ERR	(1 << 9)
#define MCI_DATA_BLK_END	(1 << 10)
#define MCI_CMD_ACTIVE		(1 << 11)
#define MCI_TX_ACTIVE		(1 << 12)
#define MCI_RX_ACTIVE		(1 << 13)
#define MCI_TX_HALF_EMPTY	(1 << 14)
#define MCI_RX_HALF_FULL	(1 << 15)
#define MCI_TX_FIFO_FULL	(1 << 16)
#define MCI_RX_FIFO_FULL	(1 << 17)
#define MCI_TX_FIFO_EMPTY	(1 << 18)
#define MCI_RX_FIFO_EMPTY	(1 << 19)
#define MCI_TX_DATA_AVAIL	(1 << 20)
#define MCI_RX_DATA_AVAIL	(1 << 21)

#define CMD_INT_MASK        (MCI_CMD_CRC_FAIL | MCI_CMD_TIMEOUT | MCI_CMD_RESP_END \
                             | MCI_CMD_SENT     | MCI_CMD_ACTIVE)

#define DATA_ERR_INT_MASK  (MCI_DATA_CRC_FAIL | MCI_DATA_TIMEOUT | MCI_TX_UNDERRUN \
                            | MCI_RX_OVERRUN | MCI_START_BIT_ERR)

#define ACTIVE_INT_MASK    ( MCI_TX_ACTIVE | MCI_RX_ACTIVE)

#define FIFO_INT_MASK      (MCI_TX_HALF_EMPTY | MCI_RX_HALF_FULL \
                            | MCI_TX_FIFO_FULL  | MCI_RX_FIFO_FULL \
                            | MCI_TX_FIFO_EMPTY | MCI_RX_FIFO_EMPTY \
                            | MCI_DATA_BLK_END )

#define	FIFO_TX_INT_MASK (MCI_TX_HALF_EMPTY )
#define	FIFO_RX_INT_MASK (MCI_RX_HALF_FULL  )

#define DATA_END_INT_MASK    (MCI_DATA_END | MCI_DATA_BLK_END)

#define ERR_TX_INT_MASK (MCI_DATA_CRC_FAIL | MCI_DATA_TIMEOUT | MCI_TX_UNDERRUN | MCI_START_BIT_ERR)
#define ERR_RX_INT_MASK (MCI_DATA_CRC_FAIL | MCI_DATA_TIMEOUT | MCI_RX_OVERRUN  | MCI_START_BIT_ERR)

/* Error code on the command response. */
#define INVALID_RESPONSE	0xFFFFFFFF

extern void MCI_TXEnable( void );
extern void MCI_TXDisable( void );
extern void MCI_RXEnable( void );
extern void MCI_RXDisable( void );

extern DWORD MCI_Init( void );
extern void  MCI_SendCmd( DWORD CmdIndex, DWORD Argument, DWORD ExpectResp, DWORD AllowTimeout );
extern DWORD MCI_GetCmdResp( DWORD CmdIndex, DWORD NeedRespFlag, DWORD *CmdRespStatus );

extern void  MCI_Set_MCIClock( DWORD clockrate );
extern DWORD SD_Set_BusWidth( DWORD width );

extern DWORD MCI_CheckStatus( void ); // mthomas: exported
extern DWORD MCI_CardInit( void );
extern DWORD MCI_Go_Idle_State( void );
extern DWORD MCI_Check_CID( void );
extern DWORD MCI_Set_Address( void );
// extern DWORD MCI_Send_CSD( void );
extern DWORD MCI_Send_CSD( DWORD* csd );
extern DWORD MCI_Select_Card( void );
extern DWORD MCI_Send_Status( void );
extern DWORD MCI_Set_BlockLen( DWORD blockLength );
extern DWORD MCI_Send_ACMD_Bus_Width( DWORD buswidth );
extern DWORD MCI_Send_Stop( void );

extern DWORD MCI_Write_Block( DWORD blockNum );
extern DWORD MCI_Write_Multiple_Block( DWORD blockNum, BYTE count, const BYTE *buff );
extern DWORD MCI_Read_Block( DWORD blockNum );

// mthomas:
extern DWORD MCI_Sd_Status( void );
extern void MCI_Power_Off( void );

#endif /* end __MCI_H */
/****************************************************************************
**                            End Of File
****************************************************************************/