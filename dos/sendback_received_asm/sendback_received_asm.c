/*
 *  Send back all received data from specified com serial port
 */

#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <process.h>
#include <stdlib.h>

#define ICR 0x20
#define IMR 0x21
#define COM 0x3f8 //COM1, 0x2f8 if COM2

#define INTVECT 0x0C //0x0B if COM2
#define IMR_MASK 0xEF //0xf7 if com2

#define IER 0x3f9
#define FIFO 0x3fa
#define LCR 0x3fb
#define MCR 0x3fc
#define LSR 0x3fd
#define MSR 0x3fe

int isr_invoke_nums;
void (interrupt *oldvect)();
unsigned char original_IMR_mask;

void (interrupt newvect)()
{
	char ch;
	isr_invoke_nums++;
	while (1){
		ch = inportb(LSR);
		if (ch & 1 == 0) break;
		ch = inportb(COM);
		outportb(COM, ch);
	}
	outportb(ICR, 0x20);
}

int main ()
{
	unsigned short port;

	outportb(IER, 0);
	printf("this program will send back bytes received from COM%c.\n", COM == 0x3f8 ? '1' : '2');
	oldvect = getvect(INTVECT);
	if (oldvect){
		printf("original vector is %p.\n");
	}else printf("original vector is NULL.\n");
	setvect(INTVECT, newvect);
	outportb(LCR, 0x80);
	outportb(COM, 0x0c);
	outportb(IER, 0);
	outportb(LCR, 0x03);
	outportb(FIFO, 0xC7);
	outportb(MCR, 0x0B);
	original_IMR_mask = inportb(IMR);
	outportb(IMR, original_IMR_mask & IMR_MASK);
	outportb(IER, 0x01);
	printf("press esc to exit.\n");
	while(1){
		if (getch() == 27) break;
	}
	outportb(IER, 0);
	outportb(IMR, original_IMR_mask);
	setvect(INTVECT, oldvect);
	printf("ISR invoked %d times.\n", isr_invoke_nums);

	return 0;
}
