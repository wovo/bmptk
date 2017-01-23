#include <stdlib.h>

extern unsigned int __heap_start;
extern unsigned int __heap_end;

int _first_free = (int) & __heap_start;
int _last_free = (int) & __heap_end;

// very simple malloc: advance the _first_free pointer
void * malloc( size_t n ){
   // round up to a multiple of 4 to keep the alignment
   n = ( n + 3 ) & ~ 0x03;
   
   int f = _first_free + n;
   if( f > _last_free ){
      return 0;
   } else {
      int p = _first_free;
      _first_free = f;
      return (void*) p;
   }
}

// using free must cause a linker error
void free( void * p ){
   void if_you_see_this_you_are_using_free_or_delete();
   if_you_see_this_you_are_using_free_or_delete();
}


