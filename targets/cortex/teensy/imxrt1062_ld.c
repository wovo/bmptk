/*
	This linker script was made by Paul Stoffregen (https://github.com/PaulStoffregen) for use with the Arduino IDE. 
	Oscar Kromhout (https://github.com/OscarKro) copied this file for use with BMPTK from WOVO (https://github.com/wovo) and edited the bootdata.c and startup.c as well.
	I also placed comments so it is more easily to read for people who are unfamiliar with linker scripts
*/

/* code to tell the linker which memory exists, on which adress it starts and how large it is */
/* https://www.pjrc.com/store/teensy40.html */
MEMORY
{
	ITCM (rwx):  ORIGIN = 0x00000000, LENGTH = 512K /* The "fastrun" part. So the part where all the code is loaded into after booting */
	DTCM (rwx):  ORIGIN = 0x20000000, LENGTH = 512K /* The Part where both the stack and the bss recide */
	RAM (rwx):   ORIGIN = 0x20200000, LENGTH = 512K /* The second but slower RAM block, used for code attributed with .dmabuffers */
	FLASH (rwx): ORIGIN = 0x60000000, LENGTH = 1984K /* The flash memory, where the flash config data, image vector (IVT), vector table (vtor) and startup code is placed. The ITCM points to this flash memory for reading out the code */
}

ENTRY(ImageVectorTable) /* the place where our image first starts, so where the program counter is directed towards at the start of the code*/

/* This code tells the linker what "sections" or "tags of code" exist, and where to place these. So if a piece of code has the attribute ".flashconfig",
this section tells that all code with attribute ".flashconfig" need to be placed in a section called ".text.progrem" which is located at the beginnen of FLASH.
*/
SECTIONS
{
	.text.progmem : {
		KEEP(*(.flashconfig)) /*place flash config at the beginning of the flash, and keep it there, even if there is no code with this attribute */
		FILL(0xFF) /* fill everything with 0xFF untill the next instruction */
		. = ORIGIN(FLASH) + 0x1000; /* place/jump to the adress of the origin of flash (0x60000000) + 1000 bytes */
		KEEP(*(.ivt)) /* place the required image vector table there (p.260)*/
		KEEP(*(.bootdata)) /* place the required bootdata array here*/
		KEEP(*(.vectors)) /* place the vector table containing the SP & PC here */
		KEEP(*(.startup)) /* place the startup code here */
		*(.flashmem*) /* place everything with the attribute .flashmem here */
		*(.progmem*) /* place everything with the attribute .progmem here */
                . = ALIGN(4); /* jump to the first adress that can be divided by four. So essentially aligning on four bytes */
                KEEP(*(.init)) /* this whole part is stuff that comes standard from the compiler, in other words, the compiler gives flags/attributes to certain parts of code that are placed here. */
                __preinit_array_start = .;
                KEEP (*(.preinit_array)) /* This section holds an array of function pointers that contributes to a single pre-initialization array for the executable or shared object containing the section.*/
                __preinit_array_end = .;
                __init_array_start = .;
                KEEP (*(.init_array)) /* This section holds an array of function pointers that contributes to a single initialization array for the executable or shared object containing the section. */
                __init_array_end = .;
		. = ALIGN(16); /* align on adresses dividable by 16 */
	} > FLASH /* place the whole code section ".text.progmem" at the start of the FLASH memory section */

	.text.itcm : {
		. = . + 32; /* MPU to trap NULL pointer deref */
		*(.fastrun) /* place all the code that have the attribute .fastrun here */
		*(.text*) /* place all the "normal" .text code here. */
		. = ALIGN(16); /* align on an adress divdiable by 16 */
	} > ITCM  AT> FLASH /* let the virtual memory adress (ITCM), point to the load memory adress (FLASH), this code is later copied from flash to the itcm at startup, by startup.c.  */

	/* this is standard arm stuff made by the compiler */
	.ARM.exidx : {
		__exidx_start = .;
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
		__exidx_end = .;
	} > ITCM  AT> FLASH  /* let the virtual memory adress (ITCM), point to the load memory adress (FLASH, where this stuff is actually flashed to). So we can copy it later from the load adress to the needed adress at dtcm */

	/* absolutely no idea why this is needed */
	.text.itcm.padding (NOLOAD) : {
		. = ALIGN(32768);
	} > ITCM
	
	.data : {
		*(.rodata*) /* place all the read only data here (initialized) */
		*(.data*) /* place all other initialized data here */
		. = ALIGN(16);
	} > DTCM  AT> FLASH /* let the virtual memory adress (DTCM) point to the load adress of flash, so when this stuff is needed, it reads it out from the flash memory, leaving the dtcm memory free for other stuff */

	.bss ALIGN(4) : {
		*(.bss*) /* place all the unitialized variables here */
		*(COMMON) /* a compiler attribute that is also added to unitialized variables, by itself refers to all uninitialized data from all input files, so that is placed here as well */
		. = ALIGN(32);
		. = . + 32; /* MPU to trap stack overflow */
	} > DTCM /* the bss will be placed in the DTCM memory section */

	.bss.dma (NOLOAD) : {
		*(.dmabuffers)
		. = ALIGN(32);
	} > RAM /* place all the code attributed with .dma buffers at RAM */

	/* All code below are "provides" for the startup.c. So here _stext for example gets the value of the adress of the .text.itcm section. _stext (start text) is then
	later used in the startup.c by some other functions. */
	_stext = ADDR(.text.itcm);
	_etext = ADDR(.text.itcm) + SIZEOF(.text.itcm) + SIZEOF(.ARM.exidx);
	_stextload = LOADADDR(.text.itcm);

	_sdata = ADDR(.data);
	_edata = ADDR(.data) + SIZEOF(.data);
	_sdataload = LOADADDR(.data);

	_sbss = ADDR(.bss);
	_ebss = ADDR(.bss) + SIZEOF(.bss);

	_heap_start = ADDR(.bss.dma) + SIZEOF(.bss.dma);
	_heap_end = ORIGIN(RAM) + LENGTH(RAM);

	_itcm_block_count = (SIZEOF(.text.itcm) + SIZEOF(.ARM.exidx) + 0x7FFF) >> 15;
	_flexram_bank_config = 0xAAAAAAAA | ((1 << (_itcm_block_count * 2)) - 1);
	_estack = ORIGIN(DTCM) + ((16 - _itcm_block_count) << 15);

	_flashimagelen = SIZEOF(.text.progmem) + SIZEOF(.text.itcm) + SIZEOF(.ARM.exidx) + SIZEOF(.data);
	_teensy_model_identifier = 0x24;

}
