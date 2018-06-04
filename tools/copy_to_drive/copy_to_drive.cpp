#include <iostream>
#include <ios>
#include <fstream>
#include <windows.h>

// copy file argv[1] to file argv[3] on drive argv[2]
// if argv[2] is one letter it is a drive letter, 
// otherwise it is a volume label

char find_volume( const char *name ){;
   char drives_list[ MAX_PATH ] = {0};
   DWORD Result = GetLogicalDriveStrings( MAX_PATH, drives_list );

   if( Result > 0 && Result <= MAX_PATH ){
      char* drive_ptr = drives_list;
      while( *drive_ptr ){

         char Label[MAX_PATH];
         DWORD DontNeedThis;
         if( GetVolumeInformation( 
            drive_ptr,
            Label,
            sizeof ( Label ),
            NULL,
            &DontNeedThis,
            &DontNeedThis,
            NULL,
            0 
         )){ 
            // std::cout << "Drive: " << drive_ptr << " = " << Label << "\n";
            
            if( 0 == strcmp( Label, name )){
               return *drive_ptr;
            }
         }         
         
         // get the next drive
         drive_ptr += strlen( drive_ptr ) + 1;
      }
      
      std::cout 
         << "error: no drive labeled '" << name << "' found.\n";
      system( "PAUSE" );         
      exit(-1);     
     
  } else {
  
      std::cout 
         << "error: GetLogicalDriveStrings failed\n";
      system( "PAUSE" );         
      exit(-1);
  }
}

int main( int argc, char *argv[] ){
   if( argc != 4 ){
   
      std::cout 
         << "error: " 
         << argv[ 0 ] 
         << " must be called with 3 arguments\n";
      system( "PAUSE" );         
      exit(-1);
      
   } else {
   
      char drive = 
         ( strlen( argv[ 2 ] ) == 1 )
         ? argv[ 2 ][ 0 ]
         : find_volume( argv[ 2 ] );
         
      std::string source = argv[ 1 ];
      std::string target = drive + std::string( ":\\" ) + argv[ 3 ];
      std::cout << "copying " << source << " to " << target << "\n";
      
      std::ifstream src( source.c_str(), std::ios::binary);
      std::ofstream dst( target.c_str(), std::ios::binary);
      dst << src.rdbuf();
      src.close();
      dst.close();
             
   }
   return 0;
}
