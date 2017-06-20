/****************************************************************************
 * NAME: SerialMAX.h
 * DATE:  7/10/2003
 * PGMR: Y. Bai  Copyright 2003 - 2007
 * DESC:   Header file for serial port interface to MAX - serial A/D
 ****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <time.h>
#define     MAXNUM 100 // Max data length
#define      BYTE         unsigned char // Data type
#define      USHORT unsigned short //
#define     COM1 0x3f8 // COM1 base address
#define     COM2 0x2f8 // COM2 base address
#define     SCLK        1 // MCR  bit1 = CLK for AD
#define     CS        2 // MCR  bit2 = CS of AD
USHORT IER    = 0x3f9; // COM1 Interrupt Enable Register
USHORT LCR   = 0x3fb; // COM1 Line Control Register
USHORT MCR  = 0x3fc; // COM1 Modem Control Register
USHORT LSR    = 0x3fd; // COM1 Line Status Register
USHORT       MSR   = 0x3fe; // COM1 Modem Status Register
static clock_t  delts = 2; // Delay 2 ms
void   Port_Init();
int      Port_Read();
void   sleepad();
void   delay(clock_t  ms);
