/*****************************************************************************
 * NAME: SerialMAXEmbed.h
 * DATE:  7/14/2003
 * PRGR: Y. Bai - Copyright 2003 - 2007
 * DESC:  Header file for serial A/D converter testing program (MAX-187)
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#define MAXNUM 100 // Max data length
#define BYTE         unsigned char // Data type
#define COM2 2f8h // COM2 base address
#define IER 2f9h // COM2 Interrupt Enable Register
#define LCR 2fbh // COM2 Line Control Register
#define MCR 2fch // COM2 Modem Control Register
#define LSR 2fdh // COM2 Line Status Register
#define MSR 2feh // COM2 Modem Status Register
#define DT 0ffh // Delay time for ADC
#define SCLK      1 // MCR  bit1 = SCLK for AD
#define CS      2 // MCR  bit2 = CS of AD
void  Port_Init();
void  c_inial();
int    Port_Read();
int    c_getad();
