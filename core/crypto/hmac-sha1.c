/* hmac-sha1.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2006-2015 Daniel Otte (bg@nerilex.org)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
 * 
 * implementation of HMAC as described in RFC2104
 * Author:      Daniel Otte
 * email:       bg@nerilex.org
 * License:     GPLv3 or later
 **/

/* 
 * hmac = hash ( k^opad , hash( k^ipad  , msg))
 */

#include <stdint.h>
#include <string.h>
#include <avr/boot.h>
#include "sha1.h"
#include "hmac-sha1.h"
#include "string_boot.h"

#define IPAD 0x36
#define OPAD 0x5C

#define SECURE_WIPE_BUFFER

BOOTLOADER_SECTION
void hmac_sha1_init(hmac_sha1_ctx_t *s, const void *key, uint16_t keylength_b){
	uint8_t buffer[SHA1_BLOCK_BYTES];
	uint8_t i;
	
    /* Crucial change: key <= 160 bits */
	memset(buffer, 0, SHA1_BLOCK_BYTES);
	memcpy_boot(buffer, key, (keylength_b+7)/8);
	
	for (i=0; i<SHA1_BLOCK_BYTES; ++i){
		buffer[i] ^= IPAD;
	}
	sha1_init(&(s->a));
	sha1_nextBlock(&(s->a), buffer);
	
	for (i=0; i<SHA1_BLOCK_BYTES; ++i){
		buffer[i] ^= IPAD^OPAD;
	}
	sha1_init(&(s->b));
	sha1_nextBlock(&(s->b), buffer);
	
	
#if defined SECURE_WIPE_BUFFER
	memset(buffer, 0, SHA1_BLOCK_BYTES);
#endif
}

BOOTLOADER_SECTION
void hmac_sha1_nextBlock(hmac_sha1_ctx_t *s, const void *block){
	sha1_nextBlock(&(s->a), block);
}
BOOTLOADER_SECTION
void hmac_sha1_lastBlock(hmac_sha1_ctx_t *s, const void *block, uint16_t length_b){
	while(length_b>=SHA1_BLOCK_BITS){
		sha1_nextBlock(&s->a, block);
		block = (uint8_t*)block + SHA1_BLOCK_BYTES;
		length_b -= SHA1_BLOCK_BITS;
	}
	sha1_lastBlock(&s->a, block, length_b);
}

BOOTLOADER_SECTION
void hmac_sha1_final(void *dest, hmac_sha1_ctx_t *s){
	sha1_ctx2hash(dest, &s->a);
	sha1_lastBlock(&s->b, dest, SHA1_HASH_BITS);
	sha1_ctx2hash(dest, &(s->b));
}
