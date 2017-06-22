/******************************************************************************
 * NAME: SerialComm.cpp
 * DATE:  7/15/2003
 * PRGM: Y. Bai - Copyright 2003 - 2007
 * DESC:  Use serial port communication between two computers. Two set of programs should
                  be developed and installed in two computers: Master and Slave computers.
   This program is for the Master computer.
 ********************************************************************************/
#include "serial_comm.h"
void main()
{
       FILE*  fp;
       int        rc, num, index, repeat = 0;
       struct serialDATA*  sdata;
       printf("\n***** WELCOME TO SERIAL PORTS COMMUNICATION *****\n");
	   printf("CLK_TCK is %d\n", CLK_TCK);
       printf("\nHow many data you want to process b/w computers? \n");
       printf("Enter an integer as the number of the data & press Enter key\n");
       scanf("%d", &index);
       fp = fopen("serial_comm.dat", "w");
       if (fp == NULL)
       {
		printf("File open error! program exit\n");
		return;
       }
       sdata = (struct serialDATA* )malloc(sizeof(struct serialDATA));
       sinial();
       rc = comm_start();
       if (rc != 0) {  printf("Error in comm_start()\n"); return;  }
       rc = send_index(index);
       if (rc != 0) {  printf("Error in send_index()\n"); return;  }
       for (num = 0;  num < index;  num++)
	   {
			rc = comm_start();
			if (rc != 0) {  printf("Error in comm_start()\n"); return;  }
			rc = comm_collect(sdata);
			printf("%d - Received pos =: %s,   ", num, sdata ->pos);
			printf("ang =: %s\n", sdata ->ang);
			fprintf(fp, "%d: %.2f    %.2f\n", num, sdata->dpos, sdata->dang);
       }
       fclose(fp);
       free(sdata);
       printf("\n************* END OF THE TEST ***************\n\n");
}

int  comm_start()
{
       BYTE signal;
       int       rc = 0;
       clock_t  start;
       start  = clock();
       signal = sends('s', NUMS);
       while((reces(NUMS) != 'c') & !kbhit())
       {
		signal = sends('s', NUMS);
		if (signal == '?')
		{
			   printf("comm_start() error\n");
			   rc = -1;
			   break;
		}
		if (elapsed >= TMOUT1)
		{
				printf("time out occurs in comm_start()\n");
				rc = TIMEOUT;
				break;
		}
	}
	return  rc;
}

int  send_index(int max)
{
        BYTE  rc;
        int        result = 0;
        char*   strIndex;
        char     buffer[8];
        short    index,  length;
        strIndex = _itoa(max, buffer, 10);
        length   = strlen(buffer);
        for (index = 0;  index <= length;  index++)
       {
			 rc = sends(buffer[index],  NUMS);
			 if (rc == '?' || rc == '!')
                {
             printf("Error in sends()\n");
                      result = -1;
                      break;
                 }
        }
        return  result;
}

int  comm_collect(struct serialDATA* res)
{
       BYTE signal;
       int  index = 0, rc = 0;
       while ((signal = reces(NUMS))!= '\0')
      {
		if (signal == '?' || signal == '!')
		{
			printf("sends() error/port is not ready\n");
			return -1;
		}
		res->pos[index] = signal;
		index++;
       }
       res ->pos[index] = '\0';
       index = 0;
       while((signal = reces(NUMS)) != '\0')
      {
		if (signal == '?' || signal == '!')
		{
			printf("sends() error/port is not ready\n");
			return -1;
		}
		res->ang[index] = signal;
		index++;
       }
       res ->ang[index] = '\0';
       res->dpos = atof(res->pos);
       res->dang = atof(res->ang);
       return rc;
}

/***************************************************************************
*               Initialization the 8250 communication port                                                              *
 ***************************************************************************/
void sinial()
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
	mov al, 00h ; b7 - b0 = (00h) = (00000000b) -> 3f9h (baud rate high-byte)
	out dx, al        ; Set up the baud rate high & low bytes (0060h -> 1200 baud rate).
	mov dx, LCR ; Finish set up the Line Controller Register.
	mov al, 0bh       ; b7 - b0 = 0bh = (00001011b) -> odd parity, 1 stop bit, 8 data bits.
	out dx, al ; Set up LCR based on the above control parameters.
	mov dx, MCR ; Set up MCR to set DTR & RTS to 1 (space or active).
	mov al, 03h       ; Self-test sub. if this line becomes "mov al,13h".
	out dx, al        ; Finish set up the MCR.
	mov dx, IER ; Set up the Interupt Enable Register.
	mov al, 0 ; No interrupt utilized.
	out dx, al        ;
	}
}

/*****************************************************************************
*                  Send handshaking signal to the slave computer                              *
 *****************************************************************************/
BYTE sends(char c, int  nums)
{
	char  feedback;
	_asm
	{
	done: mov dx, LSR     ; Check the Line Status Register.
	in al, dx         ; Inspect the state word.
	test al, 20h       ; THR is empty? (when b5 = 1: THR is empty).
	jz done          ; No, continuing to check it and waiting.
	mov dx, COM1 ; THR is empty & ready to send the next character.
	mov al, c     ; Assign character c to lower-byte in A register.
	mov feedback, al ; Return the sent-out character as a feedback
	out dx, al         ; Send the character to the 8250.
	jmp end
	mov al, '?' ; Error occurs, send '?' as a feedback
	mov feedback, al   ; Return the error information.
	end: nop
	}
	return  feedback;	
}

/****************************************************************************
*               Receiving the character from the 8250 controller.                             *
 ****************************************************************************/
BYTE reces(int  nums)
{
	char  dd;
	_asm
	{ ; Line Status Register Bit Definition
	done: mov  dx, LSR ; Check the Line Status Register    b0 = 1: Receive data is ready
	in      al, dx ;                 b1 = 1: Overrun error
	test   al, 1eh       ; Detect any error happened?  b2 = 1: Parity error
	jnz    error         ; Yes, go to error processing.         b3 = 1: Framing error
	test   al, 01h       ; The data is ready? (b0 = 1?)        b4 = 1: Break interrupt
	jz      done          ; No,continuing to check.                b5 = 1: THR is empty
	mov  dx, COM1; Data is ready                                  b6 = 1: TSR is empty
	in      al, dx         ; Get the character from the           b7      is always zero
	mov  dd, al ; Returning the character received.
	jmp  end          ;                                  
	error:mov  dx, COM1 ; Error occurs.
	in      al, dx ;
	mov   al, '?' ; Return a '?' as an indicator of the error
	mov   dd, al
	end:  nop
	}
	return dd;
}
