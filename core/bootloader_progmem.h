#ifndef BOOTLOADER_PROGMEM_H
#define BOOTLOADER_PROGMEM_H

/* This modifier moves a variable into bootloader progmem. Right now, this is
 * managed with an avr-ld flag in Makefile.include, and a complete page is
 * allocated. This can be suboptimal if a full page is not needed. Change to a
 * custom linker script to optimize this. */
#define BOOTLOADER_PROGMEM __attribute__((section(".bootmem")))

/* Compacter then pgm_read_word_far, but requires manual management of RAMPZ
 * (17th bit) + expects only a 16bit address. No hassle with 32bit variables. */
#define pgm_read_word_far_no_rampz(addr)    \
(__extension__({                            \
    uint16_t __addr16 = (uint16_t)(addr);   \
    uint16_t __result;                      \
    __asm__ __volatile__                    \
    (                                       \
        "elpm %A0, Z+"   "\n\t"             \
        "elpm %B0, Z"    "\n\t"             \
        : "=r" (__result), "=z" (__addr16)  \
        : "1" (__addr16)                    \
    );                                      \
    __result;                               \
}))

#endif
