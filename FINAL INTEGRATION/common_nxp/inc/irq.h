/******************************************************************************
 *   irq.h:  Interrupt related Header file for NXP LPC23xx/24xx Family 
 *   Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.09.01  ver 1.00    Prelimnary version, first Release
 *   2011.05.03  ver 2.00    Edits and additions for use with Eclipse/GCC
 *
******************************************************************************/
#ifndef __IRQ_H 
#define __IRQ_H
#include "type.h"

#define I_Bit			0x80
#define F_Bit			0x40

#define SYS32Mode		0x1F
#define IRQ32Mode		0x12
#define FIQ32Mode		0x11

/* To use FIQ, set below to 1;  otherwise, it's 0 */
#define FIQ				0

#define HIGHEST_PRIORITY	0x01
#define MID_PRIORITY		0x04
#define LOWEST_PRIORITY		0x0F

#define	WDT_INT			0
#define SWI_INT			1
#define ARM_CORE0_INT	2
#define	ARM_CORE1_INT	3
#define	TIMER0_INT		4
#define TIMER1_INT		5
#define UART0_INT		6
#define	UART1_INT		7
#define	PWM0_1_INT		8
#define I2C0_INT		9
#define SPI0_INT		10		/* SPI and SSP0 share VIC slot */
#define SSP0_INT		10
#define	SSP1_INT		11
#define	PLL_INT			12
#define RTC_INT			13
#define EINT0_INT		14
#define EINT1_INT		15
#define EINT2_INT		16
#define LCD_INT			16
#define EINT3_INT		17
#define	ADC0_INT		18
#define I2C1_INT		19
#define BOD_INT			20
#define EMAC_INT		21
#define USB_INT			22
#define CAN_INT			23
#define MCI_INT			24
#define GPDMA_INT		25
#define TIMER2_INT		26
#define TIMER3_INT		27
#define UART2_INT		28
#define UART3_INT		29
#define I2C2_INT		30
#define I2S_INT			31

#define VIC_SIZE		32

#define VECT_ADDR_INDEX	0x100
#define VECT_PRIO_INDEX 0x200

extern void FIQ_Handler( void )  __attribute__ ((interrupt ("FIQ"))) ;    // __irq;
void init_VIC( void );
DWORD install_irq( DWORD IntNumber, void *HandlerAddr, DWORD Priority );

/* Be aware that, from compiler to compiler, nested interrupt will have to
   be handled differently. More details can be found in Philips LPC2000
   family app-note AN10381 */
 
/* unlike Keil CARM Compiler, in ARM's RealView compiler, don't save and 
   restore registers into the stack in RVD as the compiler does that for you.
   See RVD ARM compiler Inline and embedded assemblers, "Rules for
   using __asm and asm keywords. */

#if 0       // below 3 lines are for Keil compilers only
static DWORD sysreg;		/* used as LR register */
#define IENABLE __asm { MRS sysreg, SPSR; MSR CPSR_c, #SYS32Mode }
#define IDISABLE __asm { MSR CPSR_c, #(IRQ32Mode|I_Bit); MSR SPSR_cxsf, sysreg }

#else	// for Eclipse/GCC use enable_interrupt() and disable_interrupt() below

// -------------- Additions to be used with Eclipse/GCC --------
#include <stdint.h>
//#include "inttypes.h"

#ifndef __thumb
static inline uint32_t __get_CPSR(void)
{
	uint32_t temp;
	asm volatile ("mrs %0,CPSR":"=r" (temp):) ;
	return temp;
}

static inline void __set_CPSR(uint32_t save_cpsr)
{
	asm volatile (" msr CPSR_cxsf,%0"::"r"(save_cpsr) );
}
#endif

/* enable interrupts */
static inline void enable_interrupt(void)
{
	uint32_t temp;
	temp = __get_CPSR();
	__set_CPSR(temp & ~0xC0);
}

/* Disable interrupts and save CPSR */
static inline uint32_t disable_interrupt(void)
{
	uint32_t temp;
	temp = __get_CPSR();
	__set_CPSR(temp | 0xC0);
	return temp;
}
/* Disable IRQ interrupt and save CPSR */
static inline uint32_t disable_irq(void)
{
	uint32_t temp;
	temp = __get_CPSR();
	__set_CPSR(temp | 0x80);
	return temp;
}

/* Restore interrupt status */
static inline void restore_interrupt(uint32_t saved_CPSR)
{
	__set_CPSR (saved_CPSR);
}
#endif

#endif /* end __IRQ_H */

/******************************************************************************
**                            End Of File
******************************************************************************/
