/**
 * @file startup.cpp
 * @author Oscar Kromhout (https://github.com/OscarKro)
 * @brief File containing the startup code for the IMXRT1062 chip on the Teensy 4.0 board
 * @version 0.1
 * @date 2020-10-01
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include <stdint.h>
#include "mimxrt1062.h"

// forward declaration for a main
int main(void);
// forward declaration for a start
void start();
// forward declaration of the size of our image, used in the boot_data array
extern uint32_t size_of_image;
// forward declaration of the adress of the start of the text block (From the linker script)
extern uint32_t start_adress_of_text;
// forward declaration of the adress of the end of the text block (From the linker script)
extern uint32_t end_adress_of_text;
// forward declaration of the load adress of the text block
extern uint32_t load_adress_of_text;
// forward declaration of the adress of the bss (from the linker script)
extern uint32_t start_adress_of_bss;
// forward declaration of the size of the bss (from the linker script)
extern uint32_t end_adress_of_bss;
// forward declaration of the adress of the start of the data block (From the linker script)
extern uint32_t start_adress_of_data;
// forward declaration of the adress of the end of the data block (From the linker script)
extern uint32_t end_adress_of_data;
// forward declaration of the load adress of the data block
extern uint32_t load_adress_of_data;
// forward declaration of the stack start adress
extern uint32_t stack_start_adress;
// forward declaration of the stack end adress
extern uint32_t stack_end_adress;

__attribute__ ((section(".vector_table"), used)) // compiler tag .vector_table
/**
 * @brief The vector table containing the first start adress
 * 
 */
uint32_t vector_table[2] =
	{
		(uint32_t)stack_end_adress,
		(unsigned int)&start // adress to the start up function. First image adress (pc)
};

__attribute__ ((section(".boot_data"), used)) // compiler tag .boot_data
/**
 * @brief The boot data vector. p. 262
 * 
 */
uint32_t boot_data[3] =
	{
		0x60000000,	   // Needs to be the first adress for the image
		size_of_image, // size of the total image
		0			   // some plugin image?
};

__attribute__ ((section(".IVT"), used)) // compiler tag .IVT (image vector table)
/**
 * @brief The Image Vector Table (IVT). Vector containing the most basic data adresses. p. 216
 * 
 */
uint32_t image_vector_table[8] =
	{
		0x402000D1,					  // Header adress, found in bootdata.c from arduino code. this is what paul stoffregen does, dont know why. p. 262 has short description
		(uint32_t)vector_table,	  // the docs are apparently wrong here, this should not be the first image adress, but the adress to the actual vector table
		0,							  // reserved and should be zero
		0,							  // reserved for absolute adress for the device configuration data
		(uint32_t)boot_data,							  // reserved for the absolute adress for the boot data
		(uint32_t)image_vector_table, // Absolute address of the IVT. Used internally by the ROM
		0,							  // Absolute address of the Command Sequence File (CSF) used by the HAB library
		0							  // Reserved
};									  // p. 260

__attribute__ ((section(".flashconfig"), used)) // compiler tag .flashconfig
/**
 * @brief This array was used from Paul Stoffregen's bootdata.c. It contains all the boot information for the chip and is placed on adress 0.
 * 
 */
uint32_t FlexSPI_NOR_Config[128] = {
	// 448 byte common FlexSPI configuration block, 8.6.3.1 page 223 (RT1060 rev 0)
	// MCU_Flashloader_Reference_Manual.pdf, 8.2.1, Table 8-2, page 72-75
	0x42464346, // Tag				0x00
	0x56010000, // Version
	0,			// reserved
	0x00020101, // columnAdressWidth,dataSetupTime,dataHoldTime,readSampleClksource

	0x00000000, // waitTimeCfgCommands,-,deviceModeCfgEnable
	0,			// deviceModeSeq
	0,			// deviceModeArg
	0x00000000, // -,-,-,configCmdEnable

	0, // configCmdSeqs		0x20
	0,
	0,
	0,

	0, // cfgCmdArgs			0x30
	0,
	0,
	0,

	0x00000000, // controllerMiscOption		0x40
	0x00030401, // lutCustomSeqEnable,serialClkFreq,sflashPadType,deviceType
	0,			// reserved
	0,			// reserved
	0x00200000, // sflashA1Size			0x50
	0,			// sflashA2Size
	0,			// sflashB1Size
	0,			// sflashB2Size

	0, // csPadSettingOverride		0x60
	0, // sclkPadSettingOverride
	0, // dataPadSettingOverride
	0, // dqsPadSettingOverride

	0,			// timeoutInMs			0x70
	0,			// commandInterval
	0,			// dataValidTime
	0x00000000, // busyBitPolarity,busyOffset

	0x0A1804EB, // lookupTable[0]		0x80
	0x26043206, // lookupTable[1]
	0,			// lookupTable[2]
	0,			// lookupTable[3]

	0x24040405, // lookupTable[4]		0x90
	0,			// lookupTable[5]
	0,			// lookupTable[6]
	0,			// lookupTable[7]

	0, // lookupTable[8]		0xA0
	0, // lookupTable[9]
	0, // lookupTable[10]
	0, // lookupTable[11]

	0x00000406, // lookupTable[12]		0xB0
	0,			// lookupTable[13]
	0,			// lookupTable[14]
	0,			// lookupTable[15]

	0, // lookupTable[16]		0xC0
	0, // lookupTable[17]
	0, // lookupTable[18]
	0, // lookupTable[19]

	0x08180420, // lookupTable[20]		0xD0
	0,			// lookupTable[21]
	0,			// lookupTable[22]
	0,			// lookupTable[23]

	0, // lookupTable[24]		0xE0
	0, // lookupTable[25]
	0, // lookupTable[26]
	0, // lookupTable[27]

	0, // lookupTable[28]		0xF0
	0, // lookupTable[29]
	0, // lookupTable[30]
	0, // lookupTable[31]

	0x081804D8, // lookupTable[32]		0x100
	0,			// lookupTable[33]
	0,			// lookupTable[34]
	0,			// lookupTable[35]

	0x08180402, // lookupTable[36]		0x110
	0x00002004, // lookupTable[37]
	0,			// lookupTable[38]
	0,			// lookupTable[39]

	0, // lookupTable[40]		0x120
	0, // lookupTable[41]
	0, // lookupTable[42]
	0, // lookupTable[43]

	0x00000460, // lookupTable[44]		0x130
	0,			// lookupTable[45]
	0,			// lookupTable[46]
	0,			// lookupTable[47]

	0, // lookupTable[48]		0x140
	0, // lookupTable[49]
	0, // lookupTable[50]
	0, // lookupTable[51]

	0, // lookupTable[52]		0x150
	0, // lookupTable[53]
	0, // lookupTable[54]
	0, // lookupTable[55]

	0, // lookupTable[56]		0x160
	0, // lookupTable[57]
	0, // lookupTable[58]
	0, // lookupTable[59]

	0, // lookupTable[60]		0x170
	0, // lookupTable[61]
	0, // lookupTable[62]
	0, // lookupTable[63]

	0, // LUT 0: Read			0x180
	0, // LUT 1: ReadStatus
	0, // LUT 3: WriteEnable
	0, // LUT 5: EraseSector

	0, // LUT 9: PageProgram		0x190
	0, // LUT 11: ChipErase
	0, // LUT 15: Dummy
	0, // LUT unused?

	0, // LUT unused?			0x1A0
	0, // LUT unused?
	0, // LUT unused?
	0, // LUT unused?

	0, // reserved			0x1B0
	0, // reserved
	0, // reserved
	0, // reserved

	// 64 byte Serial NOR configuration block, 8.6.3.2, page 346

	256,  // pageSize			0x1C0
	4096, // sectorSize
	1,	  // ipCmdSerialClkFreq
	0,	  // reserved

	0x00010000, // block size			0x1D0
	0,			// reserved
	0,			// reserved
	0,			// reserved

	0, // reserved			0x1E0
	0, // reserved
	0, // reserved
	0, // reserved

	0, // reserved			0x1F0
	0, // reserved
	0, // reserved
	0  // reserved
};

__attribute__ ((section(".start_utility"), used))
/**
 * @brief Function to initialize the complete bss with zero
 * 
 */
void init_bss_zero()
{
	uint32_t *d, *e;

	// clear .bss section
	d = &start_adress_of_bss;
	e = &end_adress_of_bss;
	while (d != e)
	{
		*d++ = 0;
	}
}

__attribute__ ((section(".start_utility"), used))
/**
 * @brief Function to copy memory 
 * 
 * @param source the source from where memory needs to be copied (first memory adress)	
 * @param destination the destination to copy to (first memory adress)
 * @param end_destination the end adress of the memory block (last memory adress +1 )
 */
void copy_memory(uint32_t *source, uint32_t *destination, uint32_t *end_destination)
{
	if (destination == source)
		return;
	while (destination < end_destination)
	{
		*destination++ = *source++; // copy the number at source adress to destination adress, than add 1 to both adresses
	}
}

__attribute__ ((section(".start_of_code"),used))
void set_something_in_stack()
{
  	// clear .stack section
	uint32_t *d, *e;
	d = &stack_start_adress;
	e = &stack_end_adress;
	while (d != e)
	{
		*d++ = 5;
	}
}

__attribute__ ((section(".start_of_code"), used))
/**
 * @brief This function is the entry point for the linkerscript
 * 
 */
void start()
{
	// set the bss to zero
	init_bss_zero();
	// copy the text memory to itcm
	copy_memory(&start_adress_of_text, &load_adress_of_text, &end_adress_of_text);
	// copy the data memory to dtcm
	copy_memory(&start_adress_of_data, &load_adress_of_data, &end_adress_of_data);

	//set_something_in_stack();

	//stuff to turn the light on
	IOMUXC->SW_MUX_CTL_PAD[kIOMUXC_SW_MUX_CTL_PAD_GPIO_B0_03] |= 5;
    IOMUXC_GPR->GPR27 |= 0xFFFFFFFF;
    GPIO7->GDIR |= (1 << 3);
    GPIO7->DR_SET |= (1 << 3);

	(void)main(); // call to main from hwlib

	// when returned from the main, loop till hell freezes over, but do something to eliminate unidentified behaviour
	volatile int a = 0;
	while (1)
	{
		if (a == 100000)
		{
			a = 0;
		}
		a++;
	}
}
