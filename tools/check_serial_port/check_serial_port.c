#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

// check whether argv[1] can be opened

int main( int argc, char *argv[] ){
   if( argc < 1 ){
       printf( "error: %s called without argument\n", argv[ 0 ] );
   } else {
        FILE *fp;
        fp = (FILE*) open( argv[ 1 ], O_RDWR ); // | O_NOCTTY | O_NDELAY);
        if( (int)fp == -1 ){
           printf( "The serial port %s can't be opened.\n", argv[ 1 ] );	
           printf( "Check whether it is available (device manager)\n" );
		     printf( "and not in use (other copy still running?).\n" );	
		     system( "PAUSE" );
		     exit( -1 );
        } 
        close( fp );
   }
	return 0;
}
