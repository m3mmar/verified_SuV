#ifndef MEM_LAYOUT_H
#define MEM_LAYOUT_H

#include <avr/io.h>
#include <avr/fuse.h>

#ifndef BOOTSIZE
#error "BOOTSIZE must be defined!"
#endif

#if BOOTSIZE == 1

#define FUSE_BOOTSZ (FUSE_BOOTSZ0 & FUSE_BOOTSZ1)

#define APP_META 0xFD00
#define APP_METAW 0x7E80

#define SHADOW 0xFE00
#define SHADOWW 0x7F00

#define SHADOW_META 0x1FB00
#define SHADOW_METAW 0xFD80

#define MICROVISOR 0x1FC00
#define MICROVISORW 0xFE00

#elif BOOTSIZE == 2

#define FUSE_BOOTSZ (FUSE_BOOTSZ0)

#define APP_META 0xFB00
#define APP_METAW 0x7D80

#define SHADOW 0xFC00
#define SHADOWW 0x7E00

#define SHADOW_META 0x1F700
#define SHADOW_METAW 0xFB80

#define MICROVISOR 0x1F800
#define MICROVISORW 0xFC00

#elif BOOTSIZE == 4

#define FUSE_BOOTSZ (FUSE_BOOTSZ1)

#define APP_META 0xF700
#define APP_METAW 0x7B80

#define SHADOW 0xF800
#define SHADOWW 0x7C00

#define SHADOW_META 0x1EF00
#define SHADOW_METAW 0xF780

#define MICROVISOR 0x1F000
#define MICROVISORW 0xF800

#elif BOOTSIZE == 8

#define FUSE_BOOTSZ (0xFF)

#define APP_META 0xEF00
#define APP_METAW 0x7780

#define SHADOW 0xF000
#define SHADOWW 0x7800

#define SHADOW_META 0x1DF00
#define SHADOW_METAW 0xEF80

#define MICROVISOR 0x1E000
#define MICROVISORW 0xF000

#else
#error "Incorrect BOOTSIZE value!"
#endif

#define APP_START 0x0000
#define APP_STARTW 0x0000

#define MEM_END 0x1FFFF
#define MEM_ENDW 0xFFFF

#endif
