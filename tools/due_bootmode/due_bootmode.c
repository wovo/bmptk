#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

// open the comport argv[1] at 1200 baud, wait a little, close it, wait a little more

int main( int argc, char *argv[] ){
   if( argc < 1 ){
       printf( "error: %s called without argument\n", argv[ 0 ] );
   } else {
         
       HANDLE hSerial;
       DCB dcbSerialParams = {0};
       COMMTIMEOUTS timeouts = {0};
         
       fprintf(stderr, "Opening serial port...");
       hSerial = CreateFile(
          argv[ 1 ], // "\\\\.\\COM2", 
          GENERIC_READ|GENERIC_WRITE, 0, NULL,
          OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
       if( hSerial == INVALID_HANDLE_VALUE ){
          fprintf( stderr, "Error\n" );
            return 1;
       }         
                
       fprintf(stderr, "Get settings...");
       dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
       if( GetCommState(hSerial, &dcbSerialParams) == 0 ){
           fprintf(stderr, "Error getting device state\n");
           CloseHandle(hSerial);
           return 1;
       }
       
       fprintf(stderr, "Set 1200 baud & handshakes...");
       dcbSerialParams.BaudRate = CBR_1200;
       dcbSerialParams.ByteSize = 8;
       dcbSerialParams.StopBits = ONESTOPBIT;
       dcbSerialParams.Parity   = NOPARITY;
       dcbSerialParams.fRtsControl = RTS_CONTROL_ENABLE;
       dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE;    
       if(SetCommState(hSerial, &dcbSerialParams) == 0){
           fprintf(stderr, "Error setting device parameters\n");
           CloseHandle(hSerial);
           return 1;
       }       

       fprintf(stderr, "Wait...");
       Sleep(
          200 // dwMilliseconds
       );
       
       // Close serial port
       fprintf(stderr, "Closing serial port...");
       if (CloseHandle(hSerial) == 0){
          fprintf(stderr, "Error\n");
          return 1;
       }       
       
       fprintf(stderr, "Close...");
       CloseHandle(hSerial);       

       fprintf(stderr, "Wait...");
       Sleep(
          200 // dwMilliseconds
       );
       
       fprintf(stderr, "Done.");       
   }
   return 0;
}
