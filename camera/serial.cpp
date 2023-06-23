#include "serialib/lib/serialib.h"
#include <unistd.h>
#include <stdio.h>


#if defined (_WIN32) || defined(_WIN64)
    //for serial ports above "COM9", we must use this extended syntax of "\\.\COMx".
    //also works for COM0 to COM9.
    //https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilea?redirectedfrom=MSDN#communications-resources
    #define SERIAL_PORT "\\\\.\\COM1"
#endif
#if defined (__linux__) || defined(__APPLE__)
    #define SERIAL_PORT "/dev/cu.usbserial-0001"
#endif

/*!
 * \brief main  Example of read and write string
  */
#define SERIAL_PORT "/dev/cu.usbserial-0001"
int main(int argc, char *argv[])
{
    // Serial object
    serialib serial;
	

    // Connection to serial port
    char errorOpening = serial.openDevice(SERIAL_PORT, 115200);
    // If connection fails, return the error code otherwise, display a success message
    if (errorOpening!=1){
			printf("not working");
			return errorOpening;
	}
    printf ("Successful connection to %s\n",SERIAL_PORT);
    char buffer[15] = "hello\n";
    serial.writeString(buffer);
    printf ("String sent: %s", buffer);
    serial.closeDevice();
    return 0 ;
}
