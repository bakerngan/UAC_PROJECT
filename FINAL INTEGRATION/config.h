/* System Config */

/* 
	PLL
	
	- Main clock F_OSC=14,7MHz @ Olimex LPC-P2129) [limits: 10 MHz to 25 MHz]
	- System should run at max. Frequency (60MHz) [limit: max 60 MHz]
	- Choose multiplier M=4 
	 so cclk = M * F_OSC= 4 * 14745000Hz = 58980000 Hz
	- MSEL-Bits in PLLCFG (bits 0-4) MSEL = M-1
	- F_CCO must be inbetween the limits 156 MHz to 320 MHz
	 datasheet: F_CCO = F_OSC * M * 2 * P
	- choose divider P=2 => F_CCO = 14745000Hz * 4 * 2 * 2
	 = 235920000 ~=236 MHz
	- PSEL1 (Bit6) = 0, PSEL0 (Bit5 in PLLCFG) = 1 (0b01)
*/
#define FOSC		4000000
#define PLL_M		144
#define PLL_N		4
#define MSEL		(PLL_M-1)

#define PLLE		0
#define PLLC		1

#define PLOCK		10

#define PLL_FEED1	0xAA
#define PLL_FEED2	0x55


/*
	MAM(Memory Accelerator Module)
	- chosen: MAM fully enabled = MAM-Mode 2
	- System-Clock cclk=59MHz -> 3 CCLKs are proposed as fetch timing
*/
#define MAM_MODE 	2
#define MAM_FETCH   3

/*
	VPB (V... Peripheral Bus)
	- chosen: VPB should run at full speed -> divider VPBDIV=1
	=> pclk = cclk/VPDIV_VAL = 12 MHz
*/
#define VPBDIV_VAL	4

/*
	SCB
*/
#define MEMMAP_BOOT_LOADER_MODE   0       // Interrupt vectors are re-mapped to Boot Block.
#define MEMMAP_USER_FLASH_MODE    (1<<0)  // Interrupt vectors are not re-mapped and reside in Flash.
#define MEMMAP_USER_RAM_MODE      (1<<1)  // Interrupt vectors are re-mapped to Static RAM.
