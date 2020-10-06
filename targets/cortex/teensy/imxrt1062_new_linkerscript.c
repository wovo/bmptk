/**
 * @file imxrt1062_new_linkerscript.ld
 * @author Oscar Kromhout (https://github.com/OscarKro)
 * @brief File containing the startup code for the IMXRT1062 chip on the Teensy 4.0 board
 * @version 0.1
 * @date 2020-10-01
 * 
 * @copyright Copyright (c) 2020
 * 
 */

MEMORY /*This is the "map" of the memory allocation*/
{
	ITCM (rwx):  ORIGIN = 0x00000000, LENGTH = 512K
	DTCM (rwx):  ORIGIN = 0x20000000, LENGTH = 512K
	RAM (rwx):   ORIGIN = 0x20200000, LENGTH = 512K
	FLASH (rwx): ORIGIN = 0x60000000, LENGTH = 1984K
}

ENTRY(image_vector_table) /* the place where the image starts */

SECTIONS
{
    .text : {
    KEEP(*(.flashconfig)) /* all the flash memory configuration data from startup.cpp */
    . = ORIGIN(FLASH) + 0x1000; /* Jump to where we are going to store the vector table and such */
   	KEEP(*(.IVT)) /* The image vector table from startup.cpp */
    KEEP(*(.boot_data)) /* the boot data array from startup.cpp */
    KEEP(*(.vector_table)) /* the actual vector table that would normally start on 0x0 in other cortexes from startup.cpp*/
    KEEP(*(.start_of_code)) /* The start of all the code */
	*(.start_utility) /* the code used by the startup code */
	      . = ALIGN(4);
                KEEP(*(.init))
                __preinit_array_start = .;
                KEEP(*(.preinit_array))
                __preinit_array_end = .;
                __init_array_start = .;
                KEEP(*(.init_array))
                __init_array_end = .;
		. = ALIGN(16);
    } > FLASH /* place this in the flash memory section */

    /* This section was taken (and altered) from Paul Stoffregens linkerscript.*/
    .itcm : {
		. = . + 32; /* MPU to trap NULL pointer deref */
		*(.text)
		*(.text*)
		. = ALIGN(16);
	} > ITCM AT > FLASH /* let the virtual memory adress (ITCM), point to the load memory adress (FLASH) */

    /* This section was taken (and not altered) from Paul Stoffregens linkerscript. Has something to do with ARM gnu compiler, not really sure what exactly.*/
    .ARM.exidx : {
		__exidx_start = .;
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
		__exidx_end = .;
	} > ITCM AT > FLASH /*let the virtual memory adress (ITCM), point to the load memory adress (FLASH) */

    /* This section was taken (and altered) from Paul Stoffregens linkerscript. This section holds all the initialized variables*/
    .data : {
		*(.rodata)
		*(.data)
		. = ALIGN(16);
	} > DTCM AT > FLASH /*let the virtual memory adress (DTCM), point to the load memory adress (FLASH) */

    /* This section was taken (and  altered) from Paul Stoffregens linkerscript. This section holds all the uninitialized variables*/
	.bss ALIGN(4) : {
		. = ALIGN(4); /* align on four bytes, got this from the standard bmpk_linkerscript_cortex */
		KEEP(*(.bmptk_stack)) /* bmptk_stack to be placed here */
		*(.bss*)
		*(COMMON)
		. = ALIGN(32);
		. = . + 32; /* MPU to trap stack overflow */
	} > DTCM /* place this in the DTCM memory section */
	
PROVIDE(size_of_image = SIZEOF(.text) + SIZEOF(.itcm) + SIZEOF(.ARM.exidx) + SIZEOF(.data)); /*Set the size of the image in this variable, this is used by the startup.cpp script*/
PROVIDE(start_adress_of_bss = ADDR(.bss)); /*Get the start adress of the bss, is used by the init_bss_zero function in startup.cpp */
PROVIDE(end_adress_of_bss = ADDR(.bss) + SIZEOF(.bss)); /*Get the end adress of the bss, is used by the init_bss_zero function in startup.cpp */
PROVIDE(start_adress_of_text = ADDR(.itcm)); /* get the start adress of the text block */
PROVIDE(end_adress_of_text = ADDR(.itcm) + SIZEOF(.itcm) + SIZEOF(.ARM.exidx)); /* get the end of the text block */
PROVIDE(load_adress_of_text = LOADADDR(.itcm)); /* get the load adress (because of the at keyword) from the text block */
PROVIDE(start_adress_of_data = ADDR(.data)); /* get the start adress of the data section*/
PROVIDE(end_adress_of_data = ADDR(.data) + SIZEOF(.data)); /*get the end adress of the data section*/
PROVIDE(load_adress_of_data = LOADADDR(.data)); /*get the load adress (because of the at keyword) from the data block*/
PROVIDE(stack_start_adress = ADDR(.bmptk_stack)); /* get the stack start adress */
PROVIDE(stack_end_adress = ADDR(.bmptk_stack) + SIZEOF(.bmptk_stack)); /* get the stack end adress */
}