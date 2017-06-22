/*
 *  Send back all received data from specified com serial port
 */

#include <stdio.h>
#include <conio.h>
#include <dos.h>

#define ICR 0x20
#define IMR 0x21
#define COM1 0x3f8
#define COM2 0x2f8
#define COM 0x2f8
#define IER COM+1
#define MCR COM+4
#define LCR COM+3
#define LSR COM+5
#define MSR COM+6

void c_inial (void);

int main (void)
{
	unsigned char ch;

	c_inial();
	while(!kbhit()){
		ch = _inp(LSR);
		if (ch & 1){
			_outp(COM, ch = _inp(COM));
			printf("%c", ch);
		}
	}

	return 0;
}

void c_inial (void)
{
	_asm
	{
	mov dx, LCR
	mov al, 80h
	out dx, al	;replaced by _outp(LCR, 80h)
	mov dx, COM
	mov al, 0x0C
	out dx, al	;replaced by _outp(COM, 0CH)
	mov dx, IER
	mov al, 0
	out dx, al	;above set baud rate, can replaced by _outp(IER, 0)
	mov dx, LCR
	mov al, 0bh
	out dx, al	;set no parity, 8 bits, 1 stop bit, can replaced by _outp(LCR, 0bh)
	mov dx, IER
	mov al, 0
	out dx, al	;disable interrupt
	mov dx, MSR
	mov al, 03h
	out dx, al	;set DTR/RTS
	}
}
