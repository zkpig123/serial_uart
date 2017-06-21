/******************************************************************************
 * NAME: SerialMAXEmbed.cpp
 * DATE:  7/14/2003
 * PRGR: Y. Bai - Copyright 2003 - 2007
 * DESC:  Embedded assembly code (intel 8088/80x86) to communicate with a serial A/D
                  converter (MAX-187) to get analog signal inputs from a RS-232 port
 ******************************************************************************/
#include "Serial_MAX_Embed.h"
void main()
{
        FILE*  fp;
        char      cc;
        int         index, value[100];
        double  result;
        printf("\n**** WELCOME TO SERIAL A/D CONVERTER TESTING ****\n");
        printf("\nDo you want to begin the test?  y/n\n");
        scanf("%c", &cc);
        if (cc == 'y' || cc == 'Y')
       	{
		Port_Init();
		fp = fopen("C:\\SData\\serialmaxemb.dat", "w");
		if (fp == NULL)
		{
			printf("File open error! program exit\n");
			return;
		}
		for (index = 0; index < MAXNUM; index++)
		{
			value[index] = Port_Read();
			result = ((double)value[index]/4095.0)*5.0;
				      printf("Received data =: %.2f\n", result);
		        fprintf(fp, "%.2f\n", result);
		}
		fclose(fp);
	 }
         printf("\n************* END OF THE TEST ***************\n\n");
	 }

/*****************************************************************************
 *                       Initialize the serial port COM2                                                                        *
 *****************************************************************************/
void  Port_Init()
{
        printf("\nBegin to initialize the serial port...\n");
        c_inial();
        return;
}
void  c_inial()
{
        _asm
       	{
	mov dx, MCR ; Set up MCR to set DTR & reset RTS (space or active).
	mov al,  01h      ; Enable MAX-187 (DTR = 1) & disable CS
	; (RTS = 0 is inverted by MAX-222 to 1).
	out dx, al       ; Finish set up the MCR.
       	}
}

/*****************************************************************************
  *                   Inline Assembly Function to Read Data From the Serial Port                           *
*****************************************************************************/
int  Port_Read()
{
	int  value;
	value = c_getad();
	return  value;
}

/*****************************************************************************
 *               Inline Assembly Function to Read the Converted Data                                           *
*****************************************************************************/
int  c_getad()
{
       short  rc;
	_asm
	{
	mov dx,  MCR ; Set Modem Control Register - DTR=1 & RTS=1
	mov al,   SCLK+CS  ; Begin conversion (RTS inverted to 0 by MAX-220).
	out dx,  al       ; Finish set up the MCR.
	mov cx,   0ch ; Setup the bit counter = 12 bit.
	mov bx,  00h ; Setup result register bx=0000.
	mov dx,  MSR ; Wait conversion complete
	check: in  al,   dx ; Check if bit-5 of MSR=0, it connects to DOUT of AD.
	test al,   20h ; DOUT sends a 1 when conversion is done, but this is
	; inverted to 0 by the MAX220
	jnz check ; The bit-5 on MSR is not 0, means DOUT is not 1, wait.
	again: mov   dx, DT   ; Conversion is finished. Delay a period.
	cycl1: dec   dx          ; Delay a short period of time
	jnz   cycl1
	mov   dx, MCR ;
	mov   al, CS      ; Set CLK of AD to high (DTR=0 RTS=1), both
	out   dx, al   ; DTR & RTS inverted to 1 & 0 (SCLK=1 CS=0 on AD).
	mov   dx, DT   ; Delay a short period of time
	cycl2: dec   dx          ;
	jnz   cycl2       ;
	mov   dx, MCR ; Set CLK of AD to low
	mov   al, SCLK+CS ; (DTR=RTS=1)  DTR->SCLK, RTS->CS-bar.
	out   dx, al ; SCLK=CS-bar=0 by MAX-220.
	mov   dx, DT   ; Delay a short period of time
	cycl3: dec   dx ;
	jnz   cycl3 ;
	mov   dx, MSR ; Begin input 12-bit data bit-by-bit.
	in    al, dx       ; Check if the bit-5=1 on MSR (DOUT of AD).
	test al, 20h  ; If DOUT=1,which is inverted to 0 by MAX-220.
	jnz   done        ; So a 1 in bit-5 on MSR means a 0 in DOUT received.
	inc   bx  ; An 1 in DOUT is received, increase result bx by 1.
	done: shl   bx, 1 ; Shift result left by 1 bit.
	dec   cx ; 12-bit is completed?
	jnz   again ; No, continuing on next bit
	mov dx, MCR ;
	mov al, SCLK     ; Reset MAX-187 AD converter.
	out dx, al       ;
	shr   bx, 1 ; Fix result by shifting bx right by 1 bit
	mov   rc, bx ; Return the result.
	}
       	return   rc;
}
