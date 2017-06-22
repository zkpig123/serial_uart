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
      _outp(LCR,  0x80); // Select the LCR to set up the comm. parameters.
      _outp(COM1, 0x60); // b7 - b0 = (60h) = (01100000b) -> 3f8h (baud rate low-byte)
      _outp(IER,  0); // b7 - b0 = (00h) = (00000000b) -> 3f9h (baud rate high-byte)
// (0060h -> 1200 baud rate)
      _outp(LCR,  0x0b);   // b7 - b0 = 0bh = (00001011) ->odd parity, 1 stop bit, 8 data bits.
      _outp(MCR,  0x03); // Set up MCR to set DTR & RTS to 1 (space or active).
      _outp(IER,  0); // Set up the Interupt Enable Register. No interrupt utilized.
}

/*****************************************************************************
*                  Send handshaking signal to the slave computer                              *
 *****************************************************************************/
BYTE sends(char c, int  nums)
{
         int    rc = 0;
         char fdback;
         clock_t  start;
         start  = clock();
         rc = _inp(LSR);
         while ((rc & 0x20) == 0) // data is not ready, try nums times.
        {
		rc = _inp(LSR);
		if (elapsed >= nums)
		{
			 printf("THR is not ready/time out\n");
			 fdback = '!';
			 return fdback;
		}
	}
	  _outp(COM1, c);
	  fdback = c;
	  return  fdback;
}

/****************************************************************************
*               Receiving the character from the 8250 controller.                             *
 ****************************************************************************/
BYTE reces(int  nums)
{
         char dd;
         int  rc = 0;
         clock_t  start;
         start  = clock();
         if ((_inp(LSR) & 0x1e) != 0) // error occurs.
        {
		printf("serial error in reces()\n");
		dd = '?';
		return dd;
         }
         rc = _inp(LSR);
	 while ((rc & 0x01) == 0) // data is not ready, try nums times.
         {
		rc = _inp(LSR);
		if (elapsed >= nums)
		{
			printf("data is not ready/time out\n");
			dd = '!';
			return dd;
		}
	 }
          dd = _inp(COM1); // data is ready, get it in.
          return dd;
	  }
