/*
* This file is a simulation file of all AVR built-in functions used in Microvisor.c.
* The implementation of these functions is in Assembly and already verifierd by AVR.
* Therefore, we are going to add the headers of these files here in order to be able 
* to verify our own code.
*/

#ifndef AVR_H_
#define AVR_H_

#include <string.h> /* memcpy & co */
#include <stdint.h>

/* From interrupt.h */

void cli();
	//@requires true;
	//@ ensures true;


/* From boot.h */

void boot_page_erase (uint32_t offset);
	//@requires 0 <= offset;
	//@ ensures true;


void boot_spm_busy_wait();
	//@requires true;
	//@ ensures true;


void boot_page_fill (uint32_t page_address, uint32_t word);
	//@requires true;
	//@ ensures true;


void boot_page_write(uint32_t offset);
	//@requires true;
	//@ ensures true;


void boot_rww_enable ();
	//@requires true;
	//@ ensures true;

#endif