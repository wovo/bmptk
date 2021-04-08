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


#define BMPTK_WEAK __attribute__((weak))

/* Typedef indicating interrupt service routine and appropiate pointer. */
typedef void (ISR_t)(void), (*const pISR_t)(void);


int main( void );
void fatal_error_detected( const char * );
void __startup( void );  
extern unsigned int __stack_end;			

// nr 7 (offset 0x1C) is the vectors checksum
// Cortex-m3 contains 16 internal interrupts
void (* const __vectors[ 16 ])(void) 
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

/* If SAM3X8E is used as target platform use these external interrupts */
#ifdef __SAM3X8E__

/* Define Weak ISR functions, allows non-existant definition. */
BMPTK_WEAK ISR_t SUPC_ISR;
BMPTK_WEAK ISR_t RSTC_ISR;
BMPTK_WEAK ISR_t RTC_ISR;
BMPTK_WEAK ISR_t RTT_ISR;
BMPTK_WEAK ISR_t WDG_ISR;
BMPTK_WEAK ISR_t PMC_ISR;
BMPTK_WEAK ISR_t EEFC0_ISR;
BMPTK_WEAK ISR_t EEFC1_ISR;
BMPTK_WEAK ISR_t UART_ISR;
BMPTK_WEAK ISR_t SMC_SDRAMC_ISR;
BMPTK_WEAK ISR_t SDRAMC_ISR;
BMPTK_WEAK ISR_t PIOA_ISR;
BMPTK_WEAK ISR_t PIOB_ISR;
BMPTK_WEAK ISR_t PIOC_ISR;
BMPTK_WEAK ISR_t PIOD_ISR;
BMPTK_WEAK ISR_t PIOE_ISR;
BMPTK_WEAK ISR_t PIOF_ISR;
BMPTK_WEAK ISR_t USART0_ISR;
BMPTK_WEAK ISR_t USART1_ISR;
BMPTK_WEAK ISR_t USART2_ISR;
BMPTK_WEAK ISR_t USART3_ISR;
BMPTK_WEAK ISR_t HSMCI_ISR;
BMPTK_WEAK ISR_t TWI0_ISR;
BMPTK_WEAK ISR_t TWI1_ISR;
BMPTK_WEAK ISR_t SPI0_ISR;
BMPTK_WEAK ISR_t SPI1_ISR;
BMPTK_WEAK ISR_t SSC_ISR;
BMPTK_WEAK ISR_t TC0_ISR;
BMPTK_WEAK ISR_t TC1_ISR;
BMPTK_WEAK ISR_t TC2_ISR;

// The SAM3X8E supports 30 external interrupts, see datasheet manual p. 74
__attribute__((section(".EXTI"))) pISR_t const __externalISR[ 30 ] = {
   SUPC_ISR,
   RSTC_ISR,
   RTC_ISR,
   RTT_ISR,
   WDG_ISR,
   PMC_ISR,
   EEFC0_ISR,
   EEFC1_ISR,
   UART_ISR,
   SMC_SDRAMC_ISR,
   SDRAMC_ISR,
   PIOA_ISR,
   PIOB_ISR,
   PIOC_ISR,
   PIOD_ISR,
   PIOE_ISR,
   PIOF_ISR,
   USART0_ISR,
   USART1_ISR,
   USART2_ISR,
   USART3_ISR,
   HSMCI_ISR,
   TWI0_ISR,
   TWI1_ISR,
   SPI0_ISR,
   SPI1_ISR,
   SSC_ISR,
   TC0_ISR,
   TC1_ISR,
   TC2_ISR
};

#endif // __SAM3X8E__
