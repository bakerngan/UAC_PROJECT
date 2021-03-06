/******************************************************************************
 *
 * $RCSfile: $
 * $Revision: $
 *
 * This module provides interface routines to the LPC ARM UARTs.
 * Copyright 2004, R O SoftWare
 * No guarantees, warrantees, or promises, implied or otherwise.
 * May be used for hobby or commercial purposes provided copyright
 * notice remains intact.
 *
 * reduced to see what has to be done for minimum UART-support by mthomas
 *****************************************************************************/

// #warning "this is a reduced version of the R O Software code"

#include "LPC24xx.h"
#include "uart.h"
#include <stdio.h>
//#include "type.h"
#include "target.h"
volatile int global_counter = 1;

/* on LPC210x: UART0 TX-Pin=P0.2, RX-Pin=P0.3 
   PINSEL0 has to be set to "UART-Function" = Function "01" 
   for Pin 0.2 and 0.3 */
   
#define PINSEL_BITPIN2  4
#define PINSEL_BITPIN3  6
#define PINSEL_FIRST_ALT_FUNC   1

// Values of Bits 7:4 in PINSEL to activate UART0
#define UART0_PINSEL    ((PINSEL_FIRST_ALT_FUNC<<PINSEL_BITPIN2)|  \
						 (PINSEL_FIRST_ALT_FUNC<<PINSEL_BITPIN3))
// Values of Bits 7:4 in PINMODE to turnoff pullups/downs for UART0
#define UART0_PINMODE    ((2<<PINSEL_BITPIN2)| (2<<PINSEL_BITPIN3))

// Mask to isolate UART0 in PINSEL0 and PINMODE0 regs (i.e. bits 7:4)
#define UART0_PINMASK   ((3<<PINSEL_BITPIN2)|(3<<PINSEL_BITPIN3))

/*    baudrate divisor - use UART_BAUD macro
 *    mode - see typical modes (uart.h)
 *    fmode - see typical fmodes (uart.h)
 *    NOTE: uart0Init(UART_BAUD(9600), UART_8N1, UART_FIFO_8); 
 */

void uart0Init(uint16_t baud, uint8_t mode, uint8_t fmode)
{
	 U0IER = 0x00;             	// disable all interrupts
	 U0IIR = 0x00;             	// clear interrupt ID register
	 U0LSR = 0x00;             	// clear line status register
  	 PCONP = (PCONP | 8); 		// enables uart0 on reset

	//PCLK_UART0 on reset the value is 00 so pclk = cclk/4 = 18MHz
	// setup Pin Function Select Register (Pin Connect Block) 
	// make sure old values of Bits 0-4 are masked out and
	// set them according to UART0-Pin-Selection
  	PINSEL0 = (PINSEL0 & ~UART0_PINMASK) | UART0_PINSEL;
	PINMODE0= (PINMODE0& ~UART0_PINMASK) | UART0_PINMODE;

  	// set the baudrate - DLAB must be set to access DLL/DLM
  	U0LCR = ULCR_DLAB_ENABLE; 	 // set divisor latches (DLAB)
  	U0DLL = (uint8_t)baud;       // set for baud low byte
  	U0DLM = (uint8_t)(baud >> 8);// set for baud high byte
  
  	// set the number of characters and other
  	// user specified operating parameters
  	// Databits, Parity, Stopbits - Settings in Line Control Register
  	U0LCR = (mode & ~(ULCR_DLAB_ENABLE)); // clear DLAB "on-the-fly"
  	// setup FIFO Control Register (fifo-enabled + xx trig) 
  	U0FCR = fmode;
}

int uart0Putch(int ch)
{
  while (!(U0LSR & ULSR_THRE))          // wait for TX buffer to empty
    continue;                           // also either WDOG() or swap()

  U0THR = (uint8_t)ch;  // put char to Transmit Holding Register


  return (uint8_t)ch;      // return char ("stdio-compatible"?)
}

const char *uart0Puts(const char *string)
{
	char ch;
	
	while ((ch = *string)) {
		if (uart0Putch(ch)<0) break;
		string++;
	}
	
	return string;
}

int uart0TxEmpty(void)
{
  return (U0LSR & (ULSR_THRE | ULSR_TEMT)) == (ULSR_THRE | ULSR_TEMT);
}

void uart0TxFlush(void)
{
  U0FCR |= UFCR_TX_FIFO_RESET;          // clear the TX fifo
}


/* Returns: character on success, -1 if no character is available */
int uart0Getch(void)
{
	if (U0LSR & ULSR_RDR)                 // check if character is available
		return U0RBR;                     // return character

	return -1;
}

/* Returns: character on success, waits */
int uart0GetchW(void)
{
	while ( !(U0LSR & ULSR_RDR) ); // wait for character 
	return U0RBR;                // return character
}

BYTE uart_get(void)
{
	while ( !uart_test() ) { ; }

	return U0RBR;
}

static void uart_xmit( BYTE d )
{
	while ( !( U0LSR & 0x20 ) ) {
		// wait for send possible
		;
	}
	U0THR = d;
}
int uart_test(void)
{
	return ( U0LSR & 0x01 ) ? 1 : 0;
}

// RealView syscall for libc printf
// (used during tests until eval-version-limit
//  forced early migration to GNU-tools)
int sendchar( int ch );
int sendchar( int ch )
{
	if ( ch == '\n' ) {
		uart_xmit( '\r' );
	}
	uart_xmit( ch );
	return 1;
}

void uart_put (BYTE d)
{
	sendchar( d );
}
