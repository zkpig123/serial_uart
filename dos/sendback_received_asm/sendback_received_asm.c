/*
 *  Send back all received data from specified com serial port
 */

#include <stdio.h>
#include <conio.h>
#include <dos.h>

#define ICR 0x20
#define IMR 0x21
#define COM1 3f8h
#define COM2 2f8h
#define COM COM2
#if (COM == COM1)
	#define INTVECT 0x0C
	#define IMR_MASK 0x10
#else 
	#define INTVECT 0x0B
	#define IMR_MASK 0x08
#endif
#define IER COM + 1
#define MCR COM + 4
#define LCR COM + 3
#define LSR COM + 5
#define MSR COM + 6

unsigned char original_IMR_mask;
void c_inial (void);
void (interrupt *newvect)(void);
void restore_interrupt(void);

int main (void)
{
	void (interrupt *oldvect)(void);
	uint16_t port;

	c_inial();
	set_interrupt();
	restore_interrupt();
	while(!kbhit())
		;
	restore_interrupt();

	return 0;
}

void set_interrupt(void)
{
	old_vect = getvect(INTVECT);
	setvect(INTVECT, newvect);
	_asm
	{
	mov dx, IMR
	in dx, al
	mov original_IMR_mask, al ;save original IMR_MASK
	and al, IMR_MASK
	out dx, al ;above save original imr_mask to original_IMR_mask and enable corresponding com mask bit
	mov dx, MCR
	mov al, 03h
	out dx, al	;set DTR/RTS and let interrupt enabled take effect
	mov dx, IER
	mov al, 1
	out dx, al	;interrupt enabled
	}
}

void restore_interrupt(void)
{
	_asm
	{
	mov dx, IMR
	mov al, original_IMR_mask
	out dx, al
	}
	setvect(INTVECT, oldvect);
}

void (interrupt *newvect)(void)
{
	unsigned char ch;

	_asm
	{
	mov dx, LSR
	in dx, al
	test al, 1 //RxRDY
	jz done
	mov dx, COM
	in dx, al
	out dx, al
	done:
	mov dx, ICR
	mov al, 0x20
	out dx, al
	}
}

void c_inial (void)
{
	_asm
	{
	mov dx, LCR
	mov al, 80h
	out dx, al
	mov dx, COM
	mov al, 0ch
	out dx, al
	mov dx, IER
	mov al, 0
	out dx, al	//above set baud rate
	mov dx, LCR
	mov al, 0bh
	out dx, al	//set no parity, 8 bits, 1 stop bit
	}
}
