#ifndef MEMCPY_H_
#define MEMCPY_H_

/* Identical copy of memcpy, just located in the bootloader section. */
void *memcpy_boot(void *, const void *, size_t);
int memcmp_boot(const void *, const void *, size_t);

#endif /*MEMCPY_H_*/
