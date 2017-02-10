# check that the file argv[ 1 ] has no tabs

verbose = 1

import sys
line_number = 0
file_name = sys.argv[ 1 ]
if verbose:
   print( "notab check [%s]" % file_name, end='' )
   
try:
   file = open( file_name ) 
except:
   if verbose:
      print( " - not found" )
   exit( 0 );
   
if not verbose:
   print( "notab check [%s]" % file_name, end='' )   
   
for line in file.readlines():
   line_number += 1
   position = line.find( '\t' )
   if position >= 0:
      print( 
         "\n%s:%d:%d: tab found"
         % ( file_name, line_number, position ) )
      exit( -1 );
print( " - OK" )
