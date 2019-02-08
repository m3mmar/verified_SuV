#ifndef VIRT_I_H
#define VIRT_I_H

/* Dummy function prototypes to bring ASM subroutines to C-world. Not meant to
 * be called, just used to dump address in PROGMEM */

void safe_icall_ijmp(void);
void safe_ret(void);
void safe_reti(void);

#endif
