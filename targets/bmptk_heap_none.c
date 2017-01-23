#include <stdlib.h>

// using malloc must cause a linker error
void * malloc( size_t n ){
   void if_you_see_this_you_are_using_new_or_malloc();
   if_you_see_this_you_are_using_new_or_malloc();
   return NULL;
}

// using free must cause a linker error
void free( void * p ){
   void if_you_see_this_you_are_using_free_or_delete();
   if_you_see_this_you_are_using_free_or_delete();
}


