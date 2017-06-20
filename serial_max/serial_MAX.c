/****************************************************************************
 * NAME: SerialMAX.cpp
 * DATE:  7/10/2003
 * PRGR: Y. Bai - Copyright 2003 - 2007
 * DESC:  Use C/C++ code to communicate with a serial A/D converter (MAX-187)
                  to get analog signal inputs from a RS-232 port
 *****************************************************************************/
#include "SerialMAX.h"
void main()
{
         FILE* fp;
         char     cc;
         int        index, value[100];
         double result;
         printf("\n**** WELCOME TO SERIAL A/D CONVERTER TESTING ****\n");
         printf("\nDo you want to begin the test?  y/n\n");
         scanf("%c", &cc);
         if (cc == 'y' || cc == 'Y')
        {
		Port_Init();
		fp = fopen("C:\\SData\\serialmax.dat", "w");
		if (fp == NULL)
		{
			 printf("File open error! program exit\n");
			 return;
		}
		for (index = 0; index < MAXNUM; index++)
		{
			 value[index] = Port_Read();
			 printf("Received data (int) =: %d\n", value[index]);
		//         fprintf(fp, "%d\n", value[index]);
			 result = ((double)value[index]/4095.0)*5.0;
			 printf("Received data (float) =: %.2f\n", result);
			 fprintf(fp, "%.2f\n", result);
		}
		fclose(fp);
        }
        printf("\n************* END OF THE TEST ***************\n\n");
}

void  Port_Init()
{
        printf("\nBegin to initialize the serial port...\n");
        _outp(MCR,  SCLK);
        return;
}
void sleepad()
{
         _outp(MCR,  0);
}
void delay(clock_t  ms)
{
          clock_t  ms1, ms2, index;
          if (!ms)
              return;
          if (ms <100)
         {
		do{
			  for (index=0; index<10000; index++)
			  ms -= 1;
		     }while (ms>=0);
		return;
          }
          ms1 = ms + clock();
          do{
		  ms2 = clock(); }while(ms2 < ms1);
}

int Port_Read()
{
         int i, result = 0;
         static clock_t d1ms = 0x1; // Delay 1 ms
        _outp(MCR, SCLK+CS); // Send DTR to AD to begin conversion
        while((_inp(MSR) & 32) !=0); // Wait convert complete
        for( i = 0;  i < 12;  i++)
	{
		delay(d1ms);
		_outp(MCR, CS); // Set clock high
		delay(d1ms);
		_outp(MCR, CS+SCLK); // Set clock low
		delay(d1ms);
		if ((_inp(MSR) & 32)==0) // Received data '1'
		    result += 1;
		result = result << 1;
	}

	_outp(MCR, SCLK); // Reset AD
	result = result >> 1; // Fix the final result
	return  result;
}
