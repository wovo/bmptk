//***************************************************************************
//
// file : bmptk/bmptk.h
//
// Copyright (c) 2012 ... 2014 Wouter van Ooijen (wouter@voti.nl)
// 
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// This is the one header file a user should include to
// use the bmptk core facilities.
//
//***************************************************************************

/*
int main( void );
void fatal_error_detected( const char * );
void __startup( void );  
extern unsigned int __stack_end;			

// nr 7 (offset 0x1C) is the vectors checksum
void (* const volatile __vectors[ 8 ])(void) 
   __attribute__ ((section(".vectors"))) = {
      (void (*)(void)) & __stack_end,	
		                     __startup
};

// make sure GCC doen't replace the loops by
// calls to memcpy and memset
#pragma GCC optimize ("no-tree-loop-distribute-patterns")

void __attribute__((noreturn)) __startup( void ){
   extern unsigned int __data_init_start;
   //extern unsigned int __stack_start;
   //extern unsigned int __stack_end;
   extern unsigned int __data_start;
   extern unsigned int __data_end;
   extern unsigned int __bss_start;
   extern unsigned int __bss_end;
      
	 unsigned int *s, *d, *e;

	 // clear .bss section 
	 d = & __bss_start;
	 e = & __bss_end;
	 while( d != e ){
        *d++ = 0;
	 }
	 	 
	 // copy .data section from flash to ram
	 s = & __data_init_start;
	 d = & __data_start;
	 e = & __data_end;
	 while( d != e ){ 
	    *d++ = *s++;
	 }   
  
   // call main   
   (void) main(); 
   
   // when main returns: loop till hell freezes down
   while(1){}	
}

*/s

// junk that is not needed in an embedded context, but
// is still required to satisfy the C++ compiler
void * __dso_handle;

// handle a divide-by-zero error
// the standard library invokes abort
int __aeabi_idiv0(){ return 0; }

// called when a vft entry is not yet filled in
void __cxa_pure_virtual(){}

/*
// prevent linking in the at-exit handling
int __aeabi_atexit (void *arg, void (*func) (void *), void *d){
  return 0;
}
*/


