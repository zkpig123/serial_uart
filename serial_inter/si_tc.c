/****************************************************************************
 * NAME: SerialInter.h
 * DATE:  7/18/2003
 * PGMR: Y. Bai   Copyright 2003 - 2007
 * DESC:   Header file for SerialInter.cpp.
 * NOTE:  This program can only be compiled by Microsoft C 6.0A or Turbo C++ 1.01.
 ****************************************************************************/
#include <dos.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <process.h>
#define LENGTH 1025
#define INTVECT 0x0C /* Com Port's IRQ (Must change PIC setting) */
#define USHORT unsigned short
#define BYTE unsigned char
#define ICR 0x20
#define IMR 0x21
USHORT COM1  = 0x3f8; // COM1 base address
USHORT IER  = 0x3f9; // COM1 Interrupt Enable Register
USHORT FIFO  = 0x3fa; // COM1 FIFO buffer
USHORT LCR  = 0x3fb; // COM1 Line Control Register
USHORT MCR  = 0x3fc; // COM1 Modem Control Register
USHORT LSR  = 0x3fd; // COM1 Line Status Register
USHORT MSR  = 0x3fe; // COM1 Modem Status Register
int    bufferin   = 0;
int    bufferout = 0;
char rc;
char buffer[LENGTH];
void (interrupt *oldvect)();
int isr_invoke_nums;

/* Interrupt Service Routine (ISR) for COM1 */
void (interrupt COM1INT)()
{
	BYTE  c;
	isr_invoke_nums++;
	do {
		c = inportb(LSR);
		if (c & 1)
		{
			buffer[bufferin] = inportb(COM1);
			bufferin++;
			if (bufferin == LENGTH - 1)
			bufferin = 0;
		}
	}while (c & 1);
	outportb(ICR,  0x20);
}

/*****************************************************************************
 * NAME: SerialInter.cpp
 * DATE:  7/18/2003
 * PGMR: Y. Bai   Copyright 2003 - 2007
 * DESC:  Example interruption program.
 * NOTE: This program can only be compiled by Microsoft C 6.0A or Turbo C++ 1.01.
 ****************************************************************************/
void main(void)
{
	BYTE  c;
	outportb(IER, 0); /* Turn off interrupts - COM1 */
	oldvect = getvect(INTVECT); /* Save old Interrupt Vector of later recovery */
	setvect(INTVECT, COM1INT);       /* Set Interrupt Vector Entry */
	/* COM1, COM3 - 0x0C (Interrupt vector 12) */
	/* COM2, COM4 - 0x0B (Interrupt vector 11) */
	//  PORT 1 - Communication Settings
	outportb(LCR,  0x80); /* SET DLAB ON */
	outportb(COM1, 0x0C); /* Set Baud rate - Divisor Latch Low Byte */
	/* Default 0x0C = 9,600 BPS */
	outportb(IER,  0x00); /* Set Baud rate - Divisor Latch High Byte */
	outportb(LCR,  0x03); /* 8 Bits, No Parity, 1 Stop Bit */
	outportb(FIFO, 0xC7); /* FIFO Control Register */
	outportb(MCR,  0x1B); /* Turn on DTR, RTS, and OUT2, and set in loop test */
	outportb(IMR,(inportb(IMR) & 0xEF));  /* Set Programmable Interrupt Controller */
	/* COM1, COM3 (IRQ4) - 0xEF  */
	/* COM2, COM4 (IRQ3) - 0xF7  */
	outportb(IER, 0x01); /* Interrupt when data received ¨C RXRDY */
	printf("\nProgram running... Press ESC to quit \n");
	do{
		if (bufferin != bufferout)
		{
			rc = buffer[bufferout];
			bufferout++;
			if (bufferout == LENGTH - 1) { bufferout = 0; }
			printf("%c", rc);
		}
		if (kbhit())
		{
			c = getch();
			outportb(COM1, c);
		}
	}while (c != 27); /* 27 is equivalent to ESC */
	printf("Isr run %d times.\n", isr_invoke_nums);
	outportb(IER,  0); /* Turn off interrupts - Port1 */
	outportb(IMR, (inportb(IMR) | 0x10)); /* MASK IRQ using PIC */
	/* COM1, COM3 (IRQ4) - 0x10  */
	/* COM2, COM4 (IRQ3) - 0x08  */
	setvect(INTVECT, oldvect); /* Restore old interrupt vector */
	return;
}
