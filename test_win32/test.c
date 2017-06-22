#include <windows.h>
#include <Winbase.h>
#include <stdio.h>

int main (void)
{
	HANDLE hPort;
	DCB PortDCB;
	COMMTIMEOUTS CommTimeouts;
	int ret;
	DWORD EvtMask = 0;

	// Open the serial port.
	hPort = CreateFile("\\\\.\\COM1",  // Pointer to the name of the port
	GENERIC_READ | GENERIC_WRITE,  // Access (read/write) mode
	0,              // Share mode
	NULL,          // Pointer to the security attribute
	OPEN_EXISTING, // How to open the serial port
	0,            // Port attributes
	NULL);         // Handle to port with attribute to copy
	// If it fails to open the port, return error.
	if ( hPort == INVALID_HANDLE_VALUE )
	{
		CloseHandle(hPort); // Could not open the port.
		printf("createfile failed.\n");
		getchar();
		return 1;
	}
	GetCommState(hPort, &PortDCB);   // Get the default port setting information.
	PortDCB.DCBlength = sizeof(DCB);
	// Change the DCB structure settings.
	PortDCB.BaudRate = CBR_9600;  // Current baud rate
	PortDCB.fBinary = TRUE; // Binary mode; no EOF check
	PortDCB.fParity = TRUE; // Enable parity checking.
	PortDCB.fOutxCtsFlow = FALSE; // No CTS output flow control
	PortDCB.fOutxDsrFlow = FALSE; // No DSR output flow control
	PortDCB.fDtrControl = DTR_CONTROL_ENABLE; // DTR flow control type
	PortDCB.fDsrSensitivity = FALSE; // DSR sensitivity
	PortDCB.fTXContinueOnXoff = TRUE;    // XOFF continues Tx
	PortDCB.fOutX = FALSE; // No XON/XOFF out flow control
	PortDCB.fInX = FALSE; // No XON/XOFF in flow control
	PortDCB.fErrorChar = FALSE; // Disable error replacement.
	PortDCB.fNull = FALSE; // Disable null stripping.
	PortDCB.fRtsControl = RTS_CONTROL_ENABLE;  // RTS flow control
	PortDCB.fAbortOnError = FALSE; // Do not abort reads/writes on error
	PortDCB.ByteSize = 8;  // Number of bits/bytes, 4-8
	PortDCB.Parity = NOPARITY; // 0-4=no,odd,even,mark,space
	PortDCB.StopBits = ONESTOPBIT; // 0,1,2 = 1, 1.5, 2
	// Configure the port according to the specifications of the DCB structure.
	if (!SetCommState (hPort, &PortDCB))
	{
	      printf("Unable to configure the port.\n");
	      // Free the buffer.
	      getchar();
	      return 2;
	}
	// Retrieve the time-out parameters for all read and write operations on the port.
	GetCommTimeouts(hPort, &CommTimeouts);
	// Change the COMMTIMEOUTS structure settings.
	CommTimeouts.ReadIntervalTimeout = MAXDWORD;
	CommTimeouts.ReadTotalTimeoutMultiplier = 0;
	CommTimeouts.ReadTotalTimeoutConstant = 0;
	CommTimeouts.WriteTotalTimeoutMultiplier = 5;
	CommTimeouts.WriteTotalTimeoutConstant = 100;
	// Set the time-out parameters for all read and write operations on the port.
	if (!SetCommTimeouts (hPort, &CommTimeouts))
	{
		// Could not create the read thread.
		ret = GetLastError();
		printf("Unable to set the time-out parameters & Err code = %ld\n", ret);
		getchar();
		return 2;
	}
	if (!EscapeCommFunction(hPort, SETDTR) || !EscapeCommFunction(hPort, SETRTS)){
		printf("EscapeCommFunction failed.\n");
		return -1;
	}
	if (!SetCommMask(hPort, EV_RXCHAR | EV_CTS | EV_DSR |EV_RLSD| EV_RING)){
		printf("SetCommMask failed.\n");
		getchar();
		return 0;
	}
	printf("now invoke WaitCommEvent.\n");
	if (WaitCommEvent(hPort, &EvtMask, NULL) == 0){
		printf("WaitCommEvent failed, error code:%d\n", GetLastError());
		getchar();
		return 3;
	}else{
		printf("WaitCommEvent succeeded.\n");
		printf("Returned %x.\n", EvtMask);
	}

	getchar();
	return 0;
}
