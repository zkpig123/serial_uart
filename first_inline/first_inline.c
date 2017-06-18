/*******************************************************************************
 * NAME: EmbedAssembly.cpp
 * DATE:  June 30, 2003
 * PGMR: Y. Bai - Copyright 2003-2007
 * DESC:  Serial communication test program using embedded assembly language
 *               This program uses 8250 controller to loop-test a serial port.
 *******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include <ctype.h>
#include <math.h>
#include <dos.h>
#define   COM1 3f8h // COM1 base address
#define   COM2 2f8h // COM2 base address
#define   IER 3f9h // COM1 Interrupt Enable Register
#define   MCR 3fch // COM1 Modem Control Register
#define   LCR 3fbh // COM1 Line Control Register
#define    LSR 3fdh // COM1 Line Status Register
#define    MSR 3feh // COM1 Modem Status Register
void c_inial(); // Assembly subroutine used to initialize the UART
char reces(); // Assembly subroutine used to receive a byte (uchar)
char sends(char c); // Assembly subroutine used to send out a byte
void delay(int num); // Delay subroutine used to delay a period of time
void TestSerial(); // C-code subroutine used to test the serial port
void main(void)
{
      char cc;
      printf("\n********* WELCOME TO SERIAL LOOP TESTING ***********\n\n");
      printf("Do you want to begin the testing?  y/n\n");
      scanf("%c", &cc);
      if (cc == 'y' || cc == 'Y')
         TestSerial();
      printf("************* TESTING END ****************\n");
      return;
}

/********************************************************************************
*              Subroutine to loop testing for the RS232C port.                                                               *
 ********************************************************************************/
void TestSerial()
{
      char sChar[] = {"abcdefghijklmnopqrstuvwxyz"};
      char rc, rChar;
      int  index = 0;
      c_inial();
      while(!kbhit())
      {
  rc =sends(sChar[index]);
  rChar = reces();
  if (rChar != sChar[index])
  {
         printf("Error in receiving from the port!");
         return;
  }
  else
  {
          printf("Sending   data is %c\n", sChar[index]);
          printf("Receiving data is %c\n", rChar);
  }
  index++;
  if (index == 26)
      index = 0;
      }
      return;
}

/***********************************************************************************
*               Initialization the 8250 communication port
 **********************************************************************************/
void  c_inial()
{
   _asm
     {
      mov dx, LCR ; Select the Line Control Register to set up the comm. parameters.
      mov al, 80h ; Bit7 = 1, 3f8h & 3f9h registers can access the baud rate divisor.
      out dx, al ; Nominal baud rate = 1843200/(baud rate*16).
      mov dx, COM1 ;
      mov al, 60h ; b7 - b0 = (60h) = (01100000b) -> 3f8h (baud rate low-byte)
      out dx, al        ;
      mov dx, IER ;
      mov al, 00h ; b7 - b0 = (00h) = (00000000b) -> 3f 9h (baud rate high-byte)
      out dx, al        ; Set up the baud rate high & low bytes (0060h -> 1200 baud rate).
      mov dx, LCR ; Finish set up the Line Controller Register.
      mov al, 0bh       ; b7 - b0 = 0bh = (00001011b) -> odd parity, 1 stop bit, 8 data bits.
      out dx, al ; Set up LCR based on the above control parameters.
      mov dx, MCR ; Set up Modem Control Register to set DTR & RTS to 1 (space or active).
      mov al, 03h       ; Self-test sub. if this line becomes "mov al,13h".
      out dx, al        ; Finish set up the MCR.
      mov dx, IER ; Set up the Interrupt Enable Register.
      mov al, 0 ; No interrupt utilized.
      out dx, al        ;
     }
}

