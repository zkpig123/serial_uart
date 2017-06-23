/*****************************************************************************
 * NAME : SimpleCOM.cpp
 * DESC : Simple serial port test program - single loop
 * DATE : 7/15/2003
 * PGMR : Y. Bai
 *****************************************************************************/
#include "simple_com.h"
#include "menu.h"
int main()
{
	int       index;
	unsigned char select = TRUE;
	char   userInput[16];
	char* comPort;
	ERR_CODE  ecRet = OK;
	unsigned char cPort;

	TestPassed = FALSE;
	while(select)
	{
		printf("\n");
		for (index = 0; index < MAX_MENU; index++)
		{
			PortCreateflg[index] = FALSE;
			printf(menuData[index]);
		}
		printf(menuTitle[0]);
		scanf("%s", userInput);
		cPort = (unsigned char)userInput[0];
		comPort = getMenuItem(cPort);
		printf("select = %s\n", comPort);
		if (strcmp(comPort, "EXIT") == 0 || strcmp(comPort, "NULL") == 0)
		select = FALSE;
		else
		{
			ecRet = SetupPort(comPort, (int)cPort);
			if (ecRet)
			{
				select = FALSE;
				break;
			}
			ecRet = TestPort(comPort, 1);
			if (ecRet)
			{
				select = FALSE;
				break;
			}
			if (TestPassed)
			printf("\n**** %s TEST PASSED ***\n", comPort);
		}
	}
	getchar();
}

ERR_CODE SetupPort(char* cPort, int nPort)
{
	SerialCreate *pParam;
	ERR_CODE ecRet = OK;

	pParam = malloc(sizeof(SerialCreate));
	if (pParam == NULL) return EC_MALLOC;
	pParam->lBaudRate = BAUD_RATE;
	pParam->lDataBits = NUM_BITS;
	pParam->lTimeout  = TIME_OUT;
	if (PortCreateflg[nPort])
	     return  ecRet;
	ecRet = PortInitialize(cPort, pParam);
	if (ecRet != OK)
	     printf("ERROR in PortInitialize()!\n");
	else
	{
		PortCreateflg[nPort] = TRUE;
		hPort = pParam->h_Port;
	}
	free(pParam);

	return ecRet;
}
ERR_CODE TestPort(char* cPort, BOOL display)
{
	BYTE   sByte;
	CommPortClass* comPort;
	int    numByte = NUM_BYTE, MaxByte = MAX_BYTE;
	ERR_CODE ecRet = OK;

	comPort = malloc(sizeof(CommPortClass));
	if (comPort == NULL){
		printf("malloc for CommPortClass failed.");
		TestPassed = FALSE;
		CloseHandle(hPort);
		return EC_MALLOC;
	}
	comPort->handlePort = hPort;
	comPort->iMaxChars  = NUM_BYTE;
	for (sByte = START_BYTE; sByte <= MaxByte; sByte++)
	{
		ecRet = PortWrite(hPort, sByte, numByte);
		if (ecRet)
		{
			printf("PortWrite() is failed\n");
			TestPassed = FALSE;
			CloseHandle(hPort);
			return EC_WRITE_FAIL;
		}
		if (display)
			printf("%s Port Sending:  = %c\n", cPort, sByte);
		ecRet = PortRead(comPort);
		if (ecRet)
		{
			printf("PortRead() is failed\n");
			TestPassed = FALSE;
			CloseHandle(hPort);
			return EC_READ_FAIL;
		}
		if (display)
		printf("%s Port Received: = %c\n", cPort, comPort->bByte);
		if (sByte != comPort->bByte)
		{
			TestPassed = FALSE;
			return EC_TEST_FAIL;
		}
	}
	TestPassed = TRUE;
	free(comPort);
	return ecRet;
}

char* getMenuItem(unsigned char mPort)
{
	char*    ret;
	switch (mPort)
	{
	case 'A':
	case 'a':  ret = "COM1";
	 break;
	case 'B':
	case 'b':  ret = "COM2";
	 break;
	case 'C':
	case 'c':  ret = "COM3";
	 break;
	case 'D':
	case 'd':  ret = "COM4";
	 break;
	case 'E':
	case 'e':  ret = "COM5";
	 break;
	case 'F':
	case 'f':  ret = "COM6";
	break;
	case 'G':
	case 'g':  ret = "COM7";
	 break;
	case 'H':
	case 'h':  ret = "COM8";
	 break;
	case 'X':
	case 'x':  ret = "EXIT";
	 break;
	default: printf("Invalid Selection\n");
	ret = "NULL";
	}
	return ret;
}
