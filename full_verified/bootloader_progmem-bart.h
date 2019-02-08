#ifndef BOOTLOADER_PROGMEM_H
#define BOOTLOADER_PROGMEM_H

#include <stdint.h>

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

#define RAMPZ (*((unsigned char *)0x5B))

#define pgm_read_word_far_no_rampz pgm_read_word_far_no_rampz


/*@

predicate pgm_uint(uint32_t address; uint16_t value);

lemma_auto void pgm_uint_inv();
    requires pgm_uint(?address, ?value);
    ensures pgm_uint(address, value) &*& 0 <= address &*& address + 2 < 0x20000;

predicate pgm_uints(uint32_t address, int count; list<uint16_t> values) =
    count == 0 ?
        values == nil
    :
        pgm_uint(address, ?value) &*& pgm_uints(address + 2, count - 1, ?values0) &*& values == cons(value, values0);

lemma void pgm_uints_split(uint32_t address, int index);
    requires pgm_uints(address, ?n, ?vs) &*& 0 <= index &*& index <= n;
    ensures pgm_uints(address, index, take(index, vs)) &*& pgm_uints(address + 2 * index, n - index, drop(index, vs)) &*&
        vs == append(take(index, vs), drop(index, vs));

lemma void pgm_uints_join(uint32_t address);
    requires pgm_uints(address, ?n1, ?vs1) &*& pgm_uints(address + 2 * n1, ?n2, ?vs2);
    ensures pgm_uints(address, n1 + n2, append(vs1, vs2));

@*/

uint16_t pgm_read_word_far_no_rampz(uint32_t address);
    //@ requires RAMPZ |-> ?rampz_ &*& pgm_uint(rampz_ * 0x10000 + address % 0x10000, ?value);
    //@ ensures RAMPZ |-> rampz_ &*& pgm_uint(rampz_ * 0x10000 + address % 0x10000, value) &*& result == value;

#endif
