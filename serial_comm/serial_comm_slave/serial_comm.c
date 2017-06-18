/*****************************************************************************
 * NAME: SerialComms.cpp
 * DATE:  7/16/2003
 * PRGM: Y. Bai - Copyright 2003 - 2007
 * DESC:  Use serial port communication between two computers.
                  This program is for the Slave computer.
 *****************************************************************************/
#include "serial_comm.h"
void main()
{
        FILE*  fp;
        int         rc, num, index;
        struct serialDATA*     sdata;
        struct serialSIGNAL* ssgl;
        printf("\n***** WELCOME TO SERIAL PORTS COMMUNICATION ******\n");
        fp = fopen("serialCOMs.dat", "w");
        if (fp == NULL)
        {
		printf("File open error! program exit\n");
		return;
         }
         sdata = (struct serialDATA* )malloc(sizeof(struct serialDATA));
         ssgl  = (struct serialSIGNAL* )malloc(sizeof(struct serialSIGNAL));
         sinial();
         rc = comm_start();
         if (rc != 0) { printf("Error in comm_start()\n"); return; }
         index = get_index();
         gen_signal(ssgl);
         for (num = 0; num < index; num++)
        {
		rc = comm_start();
		if (rc != 0) { printf("Error in comm_start()\n"); return; }
		rc = comm_collect(sdata,  ssgl->pos[num],  ssgl->ang[num]);
		if (rc != 0)
		{
			 printf("comm_collect() error \n");
			 break;
		}
		printf("%d - Sending pos =: %s,  ", num, sdata ->pos);
		printf("ang =: %s\n", sdata ->ang);
		fprintf(fp, "%d:   %s    %s\n", num, sdata ->pos,  sdata ->ang);
          }
          fclose(fp);
          free(sdata);
		  free(ssgl);
          printf("\n************* END OF THE TEST ***************\n\n");
}

/***************************************************************************
*                          Time Delay Subroutine                                                                       *
***************************************************************************/
void  delay(int  nums)
{
         clock_t  start;
		 int ms = 0;
         start  = clock();
         if (nums <= 0) return;
         while(elapsed <= nums)
            ms += 1;
         return;
}

int  comm_start()
{
          BYTE signal;
          int       rc = 0;
          clock_t  start;
          start = clock();
          while((reces(NUMS) != 's') & (elapsed < TMOUT1))
          {
		signal = reces(NUMS);
		if (signal == '?')
		{
			   printf("comm_start() error\n");
			   rc = -1;
			   break;
		}
          }
         signal = sends('c',  NUMS);
         if (signal == '?')
         {
               printf("Error in comm_start()\n");
               rc = -1;
         }
         return  rc;
}

int  get_index()
{
        BYTE  rc;
        int     result,  index = 0;
        char  buffer[8];
        while((rc = reces(NUMS)) != '\0')
        {
               buffer[index] = rc;
               index++;
        }
        buffer[index] = '\0';
        result = atoi(buffer);
        printf("Received index =: %d\n", result);
        return  result;
}

void  gen_signal(struct serialSIGNAL*  sgl)
{
         int  index;
         for (index = 0; index < MAXNUM; index++)
	 {
		sgl -> pos[index] = A*sin((pi/T)*index);
		if (cosFl) sgl -> ang[index] = A*cos((pi/T)*index);
		else sgl -> ang[index] = ds*((pi/T)*index);
	 }
}

int  comm_collect(struct serialDATA* res,  double  spos,  double  sang)
{
        BYTE signal;
        int  num = 0, nump,  numa;
        int  lens1,  lens2,  rc = 0;
        lens1 = sprintf(res->pos, "%.2f", spos);
        lens2 = sprintf(res->ang, "%.2f", sang);
        for (nump = 0; nump <= lens1; nump++)
        {
              signal = sends(res->pos[nump], NUMS);
              if (signal == '?' || signal == '!')
              {
                   printf("Error in comm_collect()\n");
                   return -1;
              }
        }
        for (numa = 0; numa <= lens2; numa++)
        {
              signal = sends(res->ang[numa], NUMS);
              if (signal == '?' || signal == '!')
              {
                   printf("Error in comm_collect()\n");
                   return -1;
              }
        }
        return  rc;
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
