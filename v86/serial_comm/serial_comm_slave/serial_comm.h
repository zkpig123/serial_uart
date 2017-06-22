/***************************************************************************
 * NAME: SerialComms.h
 * DATE:  7/16/2003
 * PGMR: Y. Bai  Copyright 2003 - 2007
 * DESC:   Header file for the serial communication between two computers
 ****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#define MAXNUM 100 // Max data length
#define  BYTE         unsigned char // Data type
#define USHORT unsigned short
#define  T 50 // Sine/cos signals period
#define A 10 // Sine/cos signal amplitude
#define TMOUT1 2000 // Time out value for comm_start()
#define NUMS 20 // Time out value
USHORT COM1  = 0x3f8; // COM1 base address
USHORT IER       = 0x3f9; // COM1 Interrupt Enable Register
USHORT LCR      = 0x3fb; // COM1 Line Control Register
USHORT MCR     = 0x3fc; // COM1 Modem Control Register
USHORT       LSR       = 0x3fd; // COM1 Line Status Register
USHORT       MSR      = 0x3fe; // COM1 Modem Status Register
#define elapsed ((clock() - start)/ CLK_TCK)
unsigned char     cosFl = 0;
#define pi 3.14159265
//double pi      = 3.14159265;
double ds      = 180/pi;
struct  serialDATA
{
       char  pos[64];
       char  ang[64];
};
struct  serialSIGNAL
{
       double pos[MAXNUM];
       double ang[MAXNUM];
};
void     sinial();
void     delay(int  nums);
BYTE  sends(char  c,  int  nums);
BYTE  reces(int  nums);
int        comm_start();
int        comm_collect(struct serialDATA* res,  double spos,  double sang);
int        get_index();
void     gen_signal(struct serialSIGNAL* sgl);
