//@ #include <bitops.gh>
#include <string.h>
#include <stdint.h>
#include "mem_layout.h"
#include "bootloader_progmem.h"
#include "avr.h"
#include <limits.h>
#if INT_MAX != INT16_MAX || UINTPTR_MAX != UINT16_MAX
  #error Wrong architecture! Verified only for a 16 bit architecture.
#endif

/* All Annotations */

/*** bitwise proof lemmas ***/
/*@



    lemma void Z_of_uint8_ltOReq(Z z1, Z z2)
        requires z1 == Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zsign(false), _), _), _), _), _), _), _), _)  &*& (int_of_Z(z2) == 255) ;
        ensures int_of_Z(z1) <=  int_of_Z(z2);
    {
    }

    lemma void Z_of_uint8_gtOReq(Z z1, Z z2)
        requires z1 == Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zsign(false), _), _), _), _), _), _), _), _)  &*& (int_of_Z(z2) == 0) ;
        ensures int_of_Z(z1) >=  int_of_Z(z2);
    {
    }

    lemma void Z_of_uint16_ltOReq(Z z1, Z z2)
        requires z1 == Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zsign(false), _), _), _), _), _), _), _), _), _), _), _), _), _), _), _), _) &*& (int_of_Z(z2) == UINT16_MAX) ;
        ensures int_of_Z(z1) <=  int_of_Z(z2);
    {
    }

    lemma void Z_of_uint16_gtOReq(Z z1, Z z2)
        requires z1 == Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zsign(false), _), _), _), _), _), _), _), _), _), _), _), _), _), _), _), _) &*& (int_of_Z(z2) == 0) ;
        ensures int_of_Z(z1) >=  int_of_Z(z2);
    {
    }

    lemma void Z_of_uint32_ltOReq(Z z1, Z z2)
        requires z1 == Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zsign(false), _), _), _), _), _), _), _), _), _), _), _), _), _), _), _), _), _), _), _), _), _), _), _), _), _), _), _), _), _), _), _), _) &*& (int_of_Z(z2) == UINT32_MAX) ;
        ensures int_of_Z(z1) <=  int_of_Z(z2);
    {
    }

    lemma void Z_of_uint32_gtOReq(Z z1, Z z2)
        requires z1 == Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zdigit(Zsign(false), _), _), _), _), _), _), _), _), _), _), _), _), _), _), _), _), _), _), _), _), _), _), _), _), _), _), _), _), _), _), _), _) &*& (int_of_Z(z2) == 0) ;
        ensures int_of_Z(z1) >=  int_of_Z(z2);
    {
    }
@*/

/*@
predicate state_pred(sha1_ctx_t *state, uint32_t *p, uint32_t h0, uint32_t h1, uint32_t h2, uint32_t h3, uint32_t h4,  uint64_t theLength) =
    p == state->h &*& *p |-> h0 &*& *(p+1) |-> h1 &*& *(p+2) |-> h2 &*& *(p+3) |-> h3 &*& *(p+4) |-> h4 &*& state->length |-> theLength;
  

lemma void getCharsHFromStatePred(sha1_ctx_t *state)
    requires state_pred(state, ?p, ?h0, ?h1, ?h2, ?h3, ?h4, ?theLength);
    ensures chars ((char *)(void *)p, 20, _)  &*& state->length |-> theLength &*& p == state->h;
{
    open state_pred(state, _, _, _, _, _, _, _);
    integer__to_chars(p, 4, false);
    integer__to_chars(p + 1, 4, false);
    integer__to_chars(p + 2, 4, false);
    integer__to_chars(p + 3, 4, false);
    integer__to_chars(p + 4, 4, false);
    chars_join((void *)p);
    chars_join((void *)p);
    chars_join((void *)p);
    chars_join((void *)p);

}

lemma void getStatePredFromElements(sha1_ctx_t *state)
    requires chars(?x, 20, _) &*& sha1_ctx_t_length(state,?theLength) &*& x == (char *)(state->h);
    ensures state_pred(state,?p,?h0,?h1,?h2,?h3,?h4,theLength);
{
    chars_split(x, 16);
    chars_split(x, 12);
    chars_split(x, 8);
    chars_split(x, 4);
    chars_to_integer_(x, 4, false);
    chars_to_integer_(x + 4, 4, false);
    chars_to_integer_(x + 8, 4, false);
    chars_to_integer_(x + 12, 4, false);
    chars_to_integer_(x + 16, 4, false);
    
    close state_pred(state,_,_,_,_,_,_,_);
    
}

lemma void shiftLeftNoUnderflow( uint32_t x, uint8_t y)
    requires y >= 0 &*& x >= 0 &*& y <= 255 &*& x <= UINT32_MAX; 
    ensures x << y >= 0;
{
    shiftleft_limits(x, N32, nat_of_int(y));
}

lemma void assume_shiftLeftNoOverflow_2chars( uint16_t x, uint8_t y)
    requires y >= 0 &*& x >= 0 &*& x <= 65535 &*& y <= 255; 
    ensures x << y <= 65535;
{
     assume(false);
}

lemma void assume_shiftLeftNoOverflow_1char( uint8_t x, uint8_t y)
    requires y >= 0 &*& x >= 0 &*& x <= 255 &*& y <= 255; 
    ensures x << y <= 255;
{
     assume(false);
}

lemma void shiftRightNoUnderflow( uint32_t x, uint8_t y)
    requires y >= 0 &*& x >= 0 &*& y <= 255 &*& x <= UINT32_MAX; 
    ensures x >> y >= 0;
{
    shiftright_limits(x, N32, nat_of_int(y));
}

lemma void shiftRight16NoOverflow( uint32_t x, uint8_t y)
    requires y >= 0 &*& x >= 0 &*& y <= 255  &*& x <= UINT16_MAX;
    ensures x >> y <= UINT16_MAX;
{
    shiftright_limits(x, N16, nat_of_int(y));
    pow_nat_nat_minus(2, N16, nat_of_int(y));
}

lemma void shiftRight32NoOverflow( uint32_t x, uint8_t y)
    requires y >= 0 &*& x >= 0 &*& y <= 255  &*& x <= UINT32_MAX;
    ensures x >> y <= UINT32_MAX;
{
    shiftright_limits(x, N32, nat_of_int(y));
    pow_nat_nat_minus(2, N32, nat_of_int(y));
}

lemma void OR_NoUnderflow( uint32_t x, uint32_t y)
    requires 0 <= y &*& 0 <= x &*& x <= UINT32_MAX &*& y <= UINT32_MAX;  
    ensures ((uint32_t)(x | y)) >= 0;
{  
    Z zx = Z_of_uint32(x);
    Z zy = Z_of_uint32(y);
    bitor_def(x, zx, y, zy);
    Z zt = Z_of_uint32(0);
    Z_of_uint32_gtOReq(Z_or(zx,zy), zt);
}


lemma void OR_NoOverflow( uint32_t x, uint32_t y)
    requires y >= 0 &*& x >= 0 &*& x <= UINT32_MAX &*& y <= UINT32_MAX; 
    ensures ((uint32_t)(x | y)) <= UINT32_MAX;
{  
    Z zx = Z_of_uint32(x);
    Z zy = Z_of_uint32(y);
    bitor_def(x, zx, y, zy);
    Z zt = Z_of_uint32(UINT32_MAX);
    Z_of_uint32_ltOReq(Z_or(zx,zy), zt);
}

lemma void OR_NoOverflow_2chars( uint32_t x, uint32_t y)
    requires y >= 0 &*& x >= 0 &*& x <= 65535 &*& y <= 65535; 
    ensures ((uint16_t)(x | y)) <= 65535;
{
 
    Z zx = Z_of_uint16(x);
    Z zy = Z_of_uint16(y);
    bitor_def(x, zx, y, zy);
    Z zt = Z_of_uint16(UINT16_MAX);
    Z_of_uint16_ltOReq(Z_xor(zx,zy), zt);   
}

lemma void AND_NoUnderflow( uint32_t x, uint32_t y)
    requires 0 <= y &*& 0 <= x &*& x <= UINT32_MAX &*& y <= UINT32_MAX;  
    ensures ((uint32_t)(x & y)) >= 0;
{  
    Z zx = Z_of_uint32(x);
    Z zy = Z_of_uint32(y);
    bitand_def(x, zx, y, zy);
    Z zt = Z_of_uint32(0);
    Z_of_uint32_gtOReq(Z_and(zx,zy), zt);
}

lemma void AND_NoOverflow( uint32_t x, uint32_t y)
    requires y >= 0 &*& x >= 0 &*& x <= UINT32_MAX &*& y <= UINT32_MAX; 
    ensures ((uint32_t)(x & y)) <= UINT32_MAX;
{  
    Z zx = Z_of_uint32(x);
    Z zy = Z_of_uint32(y);
    bitand_def(x, zx, y, zy);
    Z zt = Z_of_uint32(UINT32_MAX);
    Z_of_uint32_ltOReq(Z_and(zx,zy), zt);
}


lemma void N_AND_NoUnderflow( uint32_t x, uint32_t y)
    requires 0 <= y &*& 0 <= x &*& x <= UINT32_MAX &*& y <= UINT32_MAX; 
    ensures ((uint32_t)(x & ~y)) >= 0;
{  
    Z zx = Z_of_uint32(x);
    Z zy = Z_of_uint32(y);
    bitnot_def(y, zy);
    bitand_def(x, zx, int_of_Z(Z_not(zy)), Z_not(zy));
    Z zt = Z_of_uint32(0);
    Z_of_uint32_gtOReq(Z_and(zx,Z_not(zy)), zt);
}


lemma void N_AND_NoOverflow( uint32_t x, uint32_t y)
    requires y >= 0 &*& x >= 0 &*& x <= UINT32_MAX &*& y <= UINT32_MAX;
    ensures ((uint32_t)(x & ~y)) <= UINT32_MAX;
{  
    Z zx = Z_of_uint32(x);
    Z zy = Z_of_uint32(y);
    bitnot_def(y, zy);
    bitand_def(x, zx, int_of_Z(Z_not(zy)), Z_not(zy));
    Z zt = Z_of_uint32(UINT32_MAX);
    Z_of_uint32_ltOReq(Z_and(zx,Z_not(zy)), zt);
}

lemma void XOR_NoUnderflow( uint32_t x, uint32_t y)
    requires 0 <= y &*& 0 <= x &*& x <= UINT32_MAX &*& y <= UINT32_MAX; 
    ensures ((uint32_t)(x ^ y)) >= 0;
{  
    Z zx = Z_of_uint32(x);
    Z zy = Z_of_uint32(y);
    bitxor_def(x, zx, y, zy);
    Z zt = Z_of_uint32(0);
    Z_of_uint32_gtOReq(Z_xor(zx,zy), zt);
}

lemma void XOR_NoOverflow( uint32_t x, uint32_t y)
    requires y >= 0 &*& x >= 0 &*& x <= UINT32_MAX &*& y <= UINT32_MAX; 
    ensures ((uint32_t)(x ^ y)) <= UINT32_MAX;
{  
    Z zx = Z_of_uint32(x);
    Z zy = Z_of_uint32(y);
    bitxor_def(x, zx, y, zy);
    Z zt = Z_of_uint32(UINT32_MAX);
    Z_of_uint32_ltOReq(Z_xor(zx,zy), zt);
}

lemma void assume_ADD_NoOverflow_offset_pagesize( uint32_t offset, uint32_t ps)
    requires 0 <= offset &*& offset <= UINT_MAX &*& ps == 256; 
    ensures offset+ps <= UINT_MAX;
{
     assume(false);
}

lemma void assume_ADD_NoOverflow_plusplus( uint32_t x, uint32_t y)
    requires 0 <= x &*&  x <= UINT_MAX &*& y == 1; 
    ensures x+y <= UINT_MAX;
{
     assume(false);
}

lemma void assume_ADD_NoOverflow_2chars( uint16_t x, uint16_t y)
    requires 0 <= x &*&  x <= 65535 &*& 0 <= y &*&  y <= 65535; 
    ensures x+y <= 65535;
{
     assume(false);
}

lemma void assume_ADD_NoOverflow_2chars_plusplus( uint16_t x, uint32_t y)
    requires 0 <= x &*&  x <= 65535 &*& y == 1; 
    ensures x+y <= 65535;
{
     assume(false);
}

lemma void assume_ADD_NoOverflow_1char( uint8_t x, uint8_t y);
    requires 0 <= x &*& 0 <= y &*& x <= 255 &*& y <= 255; 
    ensures x+y <= 255;

lemma void assume_ADD_NoOverflow_long( uint64_t x, uint32_t y)
    requires 0 <= x &*& 0 <= y &*& x <= ULLONG_MAX &*& y <= 65535 ; 
    ensures x+y <= ULLONG_MAX;
{
     assume(false);
}

lemma void assume_16bit_pointers(void *p)
    requires true;
    ensures p <= (void *)65535;
{
     assume(false);
}

lemma void XOR_NoUnderflow_char( uint8_t x, uint8_t y)
    requires 0 <= y &*& 0 <= x &*& x <= 255 &*& y <= 255; 
    ensures ((uint8_t)(x ^ y)) >= 0;
{  
    Z zx = Z_of_uint8(x);
    Z zy = Z_of_uint8(y);
    bitxor_def(x, zx, y, zy);
    Z zt = Z_of_uint8(0);
    Z_of_uint8_gtOReq(Z_xor(zx,zy), zt);
}


lemma void XOR_NoOverflow_char( uint8_t x, uint8_t y)
    requires y >= 0 &*& x >= 0 &*& x <= 255 &*& y <= 255; 
    ensures ((uint8_t)(x ^ y)) <= 255;
{  
    Z zx = Z_of_uint8(x);
    Z zy = Z_of_uint8(y);
    bitxor_def(x, zx, y, zy);
    Z zt = Z_of_uint8(255);
    Z_of_uint8_ltOReq(Z_xor(zx,zy), zt);
}

lemma void note(bool b)
    requires b;
    ensures b;
{}


lemma_auto void ullong_to_uchars(void *p)
    requires [?f]u_llong_integer(p, _);
    ensures [f]uchars(p, sizeof(unsigned long long), _);
{
    integer__to_chars(p, sizeof(long long), false);
    chars_to_uchars(p);
}

lemma_auto void uchars_to_ullong(void *p)
    requires [?f]uchars(p, 8, _);
    ensures  [f]u_llong_integer(p, _);
{
    uchars_to_chars(p);
    chars_to_integer_(p, sizeof(long long), false);
}


lemma_auto void uchars_to_uints(void *p)
    requires [?f]uchars(p, ?n, _);
    ensures [f]integers_(p, 4, false, n/4, _);
{
    int nbInts = n / 4;
    div_rem(n, 4);
    note(0 <= nbInts);
    uchars_split(p, 4 * nbInts);
    uchars_to_integers_(p, 4, false, nbInts);
    leak [_]uchars(p + 4 * nbInts, _, _);
}

lemma_auto void uints_to_uchars(void *p)
    requires [?f]integers_(p,4,false,?n,_);
    ensures [f]uchars(p, n * sizeof(unsigned int)*2, _);
{
    integers__to_uchars(p);
}



@*/


/**************************************************************************************************************************/



/* sha1.c */
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
 * \file	sha1.c
 * \author	Daniel Otte
 * \date	2006-10-08
 * \license GPLv3 or later
 * \brief SHA-1 implementation.
 *
 */

#define LITTLE_ENDIAN

#define SHA1_HASH_BITS  160
#define SHA1_HASH_BYTES 20
#define SHA1_BLOCK_BITS 512
#define SHA1_BLOCK_BYTES 64


/** \typedef sha1_ctx_t
 * \brief SHA-1 context type
 * 
 * A vatiable of this type may hold the state of a SHA-1 hashing process
 */



typedef struct {
    uint32_t h[5];
    uint64_t length;
} sha1_ctx_t;

/********************************************************************************************************/

/**
 * \brief initialises given SHA-1 context
 *
 */

void sha1_init(sha1_ctx_t *state)
    //@requires state_pred(state,?p,_,_,_,_,_,_);
    //@ensures state_pred(state,p,0x67452301,0xefcdab89,0x98badcfe,0x10325476,0xc3d2e1f0,0);

{
    //@open state_pred(state,p,_,_,_,_,_,_);
    state->h[0] = 0x67452301;
    state->h[1] = 0xefcdab89; 
    state->h[2] = 0x98badcfe; 
    state->h[3] = 0x10325476;
    state->h[4] = 0xc3d2e1f0;
    state->length = 0;

    //@ close state_pred(state,p,0x67452301,0xefcdab89,0x98badcfe,0x10325476,0xc3d2e1f0,0);
}


/********************************************************************************************************/
/* some helping functions */

uint32_t rotl32(uint32_t n, uint8_t bits)
    //@ requires bits <= 32;
    //@ ensures 0 <= result &*& result <= UINT32_MAX;
{
    //@ produce_limits(n);
    //@ produce_limits(bits);
    uint32_t result = 0;
    uint32_t temp1 = /*@ truncating @*/ (n << bits);
    //@ shiftRightNoUnderflow(n , 32-bits);
    //@ shiftRight32NoOverflow(n , 32-bits);
    uint32_t temp2 = n >>(32-bits);
    
    //@ produce_limits(temp1);
    
    //@ OR_NoUnderflow(temp1,temp2);
    //@ OR_NoOverflow(temp1,temp2);
    result = (temp1 | temp2);

    return result;
}


uint32_t change_endian32(uint32_t x)
    //@ requires true;
    //@ ensures 0 <= result &*& result <= UINT32_MAX;

{   
    //@ produce_limits(x);
    uint32_t result = 0;
    //@ shiftRightNoUnderflow(x , 24);
    //@ shiftRight32NoOverflow(x , 24);
    uint32_t temp1 = x>>24;
    uint32_t temp2 = /*@ truncating @*/ (x<<24);
    //@ AND_NoUnderflow(x,0x0000ff00);
    //@ AND_NoOverflow(x,0x0000ff00);
    uint32_t temp3 = /*@ truncating @*/ ((x & 0x0000ff00)<<8);
    //@ AND_NoUnderflow(x,0x00ff0000);
    //@ AND_NoOverflow(x,0x00ff0000);
    //@ shiftRightNoUnderflow(x & 0x00ff0000 , 8);
    //@ shiftRight32NoOverflow(x & 0x00ff0000 , 8);
    uint32_t temp4 = (x & 0x00ff0000) >> 8;

    //@ produce_limits(temp1);
    //@ produce_limits(temp2);
    //@ produce_limits(temp4);

    //@ OR_NoUnderflow(temp1,temp2);
    //@ produce_limits(temp3);
    //@ OR_NoOverflow(temp1,temp2);
    //@ OR_NoUnderflow(temp1|temp2, temp3);
    //@ OR_NoOverflow(temp1|temp2, temp3);
    //@ OR_NoUnderflow(temp1|temp2|temp3, temp4);
    //@ OR_NoOverflow(temp1|temp2|temp3, temp4);
    result = temp1 | temp2 | temp3 | temp4;

    return result;
}


/* three SHA-1 inner functions */

uint32_t ch(uint32_t x, uint32_t y, uint32_t z)
    //@ requires 0 <= x &*& 0 <= y &*& 0 <= z;
    //@ ensures 0 <= result &*& result <= UINT32_MAX;
{
    //@ produce_limits(x);
    //@ produce_limits(y);
    //@ produce_limits(z);

    //@ AND_NoUnderflow(x,y);
    //@ AND_NoOverflow(x,y);
    //@ N_AND_NoUnderflow(z,x);
    //@ N_AND_NoOverflow(z,x);
    //@ XOR_NoUnderflow(x&y,z& ~x);
    //@ XOR_NoOverflow(x&y,z& ~x);
    uint32_t result = ((x&y)^(z& ~x));

    return result;
}


uint32_t maj(uint32_t x, uint32_t y, uint32_t z)
    //@ requires 0 <= x &*& 0 <= y &*& 0 <= z;
    //@ ensures 0 <= result &*& result <= UINT32_MAX;

{
    //@ produce_limits(x);
    //@ produce_limits(y);
    //@ produce_limits(z);

    //@ AND_NoUnderflow(x,y);
    //@ AND_NoOverflow(x,y);
    //@ AND_NoUnderflow(x,z);
    //@ AND_NoOverflow(x,z);
    //@ AND_NoUnderflow(y,z);
    //@ AND_NoOverflow(y,z);
    //@ XOR_NoUnderflow(x&y, x&z);
    //@ XOR_NoOverflow(x&y, x&z);
    //@ XOR_NoUnderflow((x&y)^(x&z), y&z);
    //@ XOR_NoOverflow((x&y)^(x&z), y&z);
    uint32_t result = ((x&y)^(x&z)^(y&z));

    return result;
}


uint32_t parity(uint32_t x, uint32_t y, uint32_t z)
    //@ requires 0 <= x &*& 0 <= y &*& 0 <= z;
    //@ ensures 0 <= result &*& result <= UINT32_MAX;
{
   
    //@ produce_limits(x);
    //@ produce_limits(y);
    //@ produce_limits(z);
    
    //@ XOR_NoUnderflow(x, y);
    //@ XOR_NoOverflow(x, y);
    //@ XOR_NoUnderflow(x^y, z);
    //@ XOR_NoOverflow(x^y, z);
    uint32_t result = ((x^y)^z);

    //@ produce_limits(result);
    return result;
}

/********************************************************************************************************/

/**
 * \brief "add" a block to the hash
 * This is the core function of the hash algorithm. To understand how it's working
 * and what thoese variables do, take a look at FIPS-182. This is an "alternativ" implementation
 */

#define MASK 0x0000000f

//typedef uint32_t (*pf_t)(uint32_t x, uint32_t y, uint32_t z);

uint32_t pf_t(uint8_t i, uint32_t x, uint32_t y, uint32_t z)
//@ requires true;
//@ ensures 0 <= result &*& result <= UINT32_MAX;
{

    //@ produce_limits(i);
    //@ produce_limits(x);
    //@ produce_limits(y);
    //@ produce_limits(z);



    uint32_t result = 0;
    switch(i)
    {
        case 0: result =  ch(x, y, z);
            break;
        case 1: result =  parity(x, y, z);
            break;
        case 2: result =  maj(x, y, z);
            break;
        case 3: result =  parity(x, y, z);
    }

    //@ produce_limits(result);
    return result;
}

void fillK (uint32_t* k)
    //@ requires k[..4] |-> _;
    //@ ensures k[..4] |-> _;
{
    
    k[0] = 0x5a827999;
    k[1] = 0x6ed9eba1;
    k[2] = 0x6f1bbcdc;
    k[3] = 0x6a62c1d6;
}

/**************** SHA1 Next Block **********************/

void sha1_nextBlock (sha1_ctx_t *state, const void *block)
        
        //@ requires  state_pred(state,_,_,_,_,_,_,?stateLength) &*& [?f]((uint32_t *)block)[..?len] |-> _ &*& 16 <= len;
        //@ ensures   state_pred(state,_,_,_,_,_,_,stateLength + 512) &*& [f]((uint32_t *)block)[..len] |-> _;

        
{
    uint32_t a[5];
    uint32_t w[16];
    uint32_t temp[1];
    uint32_t x;
    uint8_t t,s,fi, fib;
    uint8_t div = 4;
    uint32_t k[4];
    fillK(k);
    uint8_t lenB = 16;
        

    
    /* load the w array (changing the endian and so) */
    for(t=0; t<lenB; ++t)
        //@ invariant t >= 0 &*&  t <= lenB &*& w[..16] |-> _ &*& [f]((uint32_t *)block)[..len] |-> _;
    {
        
        x = ((uint32_t*)block)[t];
        uint32_t temp0 = change_endian32(x);
        w[t] = temp0;
        
    }
    //@ getCharsHFromStatePred(state);
    /* load the state */
    memcpy(a, state->h, 5*sizeof(uint32_t));
   
    /* the fun stuff */
    for(fi=0,fib=0,t=0; t<=79; ++t)
        //@ invariant t >= 0  &*& w[..16] |-> _  &*& temp[..1] |-> _  &*&  k[0..4] |-> _ &*& fi >= 0 &*&  fib < 20 &*& fi <=3 &*& a[..5] |-> _;
        
    {
        s = (uint8_t)(t & MASK);
        if(t>=16){
            uint32_t wTemp = rotl32( w[(s+13)&MASK] ^ w[(s+8)&MASK] ^
                 w[(s+ 2)&MASK] ^ w[s] ,1);
            w[s] = wTemp;
        }
        
        
        uint32_t dtemp = pf_t(fi,a[1],a[2],a[3]);
        uint32_t rtemp = rotl32(a[0],5);
        uint32_t a4 = a[4];
        uint32_t ws = w[s];
        uint32_t kfi = k[fi];
        
        //@ produce_limits(dtemp);
        //@ produce_limits(rtemp);
        //@ produce_limits(a4);
        //@ produce_limits(ws);
        //@ produce_limits(kfi);
        
  
        temp[0] = /*@ truncating @*/ ((/*@ truncating @*/ ((/*@ truncating @*/ ((/*@ truncating @*/ (rtemp + dtemp)) + a4)) + kfi)) + ws);
        
        //@ integers__to_chars(a);
        uint32_t* pa1 = a;
        memmove(pa1+1, a, 4*sizeof(uint32_t)); /* e=d; d=c; c=b; b=a; */
        a[0] = temp[0];

        pa1 = a;
        //@ chars_to_integers_(pa1+1, 4, false, 4);
        //@ open integers_(pa1+1,4,false, 4, _);
                            
        uint32_t temp2 = rotl32(*(pa1+2) ,30);
        a[2] = temp2; /* we might also do rotr32(c,2) */

        fib++;
        if(fib==20){
            fib=0;
            fi++;
            if (fi >= 4) fi = 0;
            
            
        }
    }
    //@ getStatePredFromElements(state);
    //@ open state_pred(state,_,_,_,_,_,_,_);
    
    /* update the state */
    for(t=0; t<5; ++t)
        /*@ invariant t >= 0 &*& a[..5] |-> _ &*& state->h[..5] |-> _;  @*/
    {   
        uint32_t temp_h = state->h[t];
        //@ produce_limits(temp_h);
        uint32_t temp_a = a[t];
        //@ produce_limits(temp_a);
        state->h[t] = /*@ truncating @*/ (state->h[t] + a[t]);
    }
    //@ assume_ADD_NoOverflow_long(state->length, 512);
    state->length += 512;
    //@ close state_pred(state,_,_,_,_,_,_,_);
    
}


/***************************** SHA1 Last Block **************************/

void sha1_lastBlock(sha1_ctx_t *state, const void *block, uint16_t length)
        /*@
        requires  state_pred(state,_,_,_,_,_,_,_) &*& [?f]uchars(block, ?n_bytes, _) &*& 0 <= length 
                        &*& length <= n_bytes*8 &*& length <= UINT_MAX;
        @*/
        //@ ensures   state_pred(state,_,_,_,_,_,_,_) &*& [f]uchars(block, n_bytes, _);


{
    uint8_t lb[SHA1_BLOCK_BYTES]; /* local block */
    //@ void *block0 = block;
    //@ int bytesDone = 0;
    while(length>=SHA1_BLOCK_BITS)
        //@ invariant state_pred(state,_,_,_,_,_,_,_) &*& [f]uchars(block0, n_bytes, _) &*& 0 <= bytesDone &*& block == block0 + bytesDone &*& 0 <= length &*& length <= (n_bytes - bytesDone) * 8;
    {
        //@ uchars_split(block0, bytesDone);
        //@ uchars_split(block, SHA1_BLOCK_BYTES);
        //@ uchars_to_integers_(block, 4, false, 16);
        //@ div_rem(SHA1_BLOCK_BYTES, 4);
        sha1_nextBlock(state, block);
        //@ integers__to_uchars(block);
        length -= SHA1_BLOCK_BITS;
        block = (uint8_t*)block + SHA1_BLOCK_BYTES;
        //@ uchars_join(block0);
        //@ bytesDone += 64;
    }
    //@ open state_pred(state,_,_,_,_,_,_,_);
    //@ assume_ADD_NoOverflow_long(state->length, length);
    state->length += length;
    //@ close state_pred(state,_,_,_,_,_,_,_);
    memset(lb, 0, SHA1_BLOCK_BYTES);

    //@ div_rem(length+7, 8);
    uint16_t size_to_copy =  (uint16_t)((length+7)/8); //>>3
   
    //@ chars_split(lb, size_to_copy);
    //@ uchars_split(block0, block - block0);
    //@ uchars_to_chars(block);
    //@ chars_split(block, size_to_copy);
    memcpy (lb, block, size_to_copy);
    //@ chars_join(lb);
    //@ chars_join(block);
    //@ chars_to_uchars(block);

    /* set the final one bit */
    //@ div_rem(length, 8);
    uint32_t size_to_update = (uint32_t) length/8; //>>3
    //@ div_rem(length, 8);
    //@ chars_to_uchars(lb);
    uint16_t temp_length = length;
    //@ produce_limits(temp_length);
    uint8_t temp_lb = lb[size_to_update];
    //@ produce_limits(temp_lb);
    //@ AND_NoUnderflow( temp_length, 0x07);
    //@ AND_NoOverflow( temp_length, 0x07);
    uint16_t temp_and = (uint16_t)(temp_length & 0x07);
    //@ produce_limits(temp_and);
    //@ shiftRightNoUnderflow(0x80, temp_and);
    //@ shiftRight16NoOverflow(0x80, temp_and);
    uint8_t temp_op = /*@ truncating @*/(uint8_t) (0x80>>temp_and);
    //@ produce_limits(temp_op);
    //@ OR_NoUnderflow(lb[size_to_update],temp_op);
    lb[size_to_update] |= temp_op;            
    //@ div_rem(length, 8);
    if (length>(uint16_t)(512-64-1)){ /* not enough space for 64bit length value */
        //@ uchars_to_integers_(lb,4,false,16);//uchars_to_uints(lb);
        //@ div_rem(64,4);
        sha1_nextBlock(state, lb);
        //@ integers__to_uchars(lb); //uints_to_uchars(lb);
        //@ open state_pred(state,_,_,_,_,_,_,_);
        state->length -= 512;
        //@close state_pred(state,_,_,_,_,_,_,_);
        memset(lb, 0, SHA1_BLOCK_BYTES);
        //@chars_to_uchars(lb); 
    }
    /* store the 64bit length value */
#if defined LITTLE_ENDIAN

    //@ open state_pred(state,_,_,_,_,_,_,_);

    	uint64_t *p_state_length = &state->length;
    	//@ produce_limits(p_state_length);
        uint64_t length_address = (uint64_t)p_state_length;
        
        uint8_t* pl = (uint8_t*)length_address;               
        //@ close state_pred(state,_,_,_,_,_,_,_);

        uint8_t val[8];
   
        //@ open state_pred(state,_,_,_,_,_,_,_);
        //@ open sha1_ctx_t_length(state,_);
        //@ ullong_to_uchars(pl);      
        fill_values(pl,val);
        //@ uchars_to_ullong(pl);       
        //@ close sha1_ctx_t_length(state,_);
        //@ close state_pred(state,_,_,_,_,_,_,_); 
       
    //@ uchars_split(lb,64); 
    //@ uchars_to_integers_(lb,4,false,16); //uchars_to_uints(lb);
    //@ div_rem(64,4);    
    for (uint8_t i=0; i<8; ++i)
        //@ invariant i >= 0 &*&  state_pred(state,_,_,_,_,_,_,_) &*& lb[..64] |-> _ &*& uchars(val,8,_);
    {
        lb[56+i] = val[7-i];
        //@ div_rem(64,4);    
        //@ uchars_to_integers_(lb,4,false,16); //uchars_to_uints(lb);
    }
    //@ uchars_join(lb);


#elif defined BIG_ENDIAN
    *((uint64_t)&(lb[56])) = state->length;
#endif
    //@ uchars_to_integers_(lb,4,false,16); //uchars_to_uints(lb);
    //@ div_rem(64,4);
    sha1_nextBlock(state, lb);
    //@ integers__to_uchars(lb); //uints_to_uchars(lb);   

}


void fill_values(uint8_t * pointer, uint8_t * array)
    //@ requires uchars(array,8,_) &*& uchars(pointer,8,?cs);
    //@ ensures uchars(array,8,_) &*& uchars(pointer,8,cs);
{
    
    for (uint8_t i = 0; i < 7; i++)
        //@ invariant i >= 0 &*& uchars(array,8,_) &*& uchars(pointer,8,cs); 
    {
        array[i] = pointer [i];
    }
}

/************************************************************************/

/********************************************************************************************************/


void sha1_ctx2hash (void *dest, sha1_ctx_t *state)
    //@ requires  state_pred(state,_,_,_,_,_,_,_) &*& ((uint32_t *)dest)[..5] |-> _;
    //@ ensures state_pred(state,_,_,_,_,_,_,_) &*& ((uint32_t *)dest)[..5] |-> _;
{
#if defined LITTLE_ENDIAN
    uint8_t i;
    for(i=0; i<5; ++i)
        //@ invariant i >= 0 &*& state_pred(state,_,_,_,_,_,_,_) &*& ((uint32_t *)dest)[..5] |-> _;
    {
    	//@ getCharsHFromStatePred(state);
        //@ chars_to_integers_(state->h,4,false,5); //chars_to_uints(state->h,5);
        uint32_t temp = change_endian32(state->h[i]);
        ((uint32_t*)dest)[i] = temp;
        //@ integers__to_chars(state->h);//uints_to_chars(state->h);
        //@ getStatePredFromElements(state);
    }
    
#elif BIG_ENDIAN
    if (dest != state->h)
        memcpy(dest, state->h, SHA1_HASH_BITS/8);
#else
# error unsupported endian type!
#endif
}


/* ************************************************************************************************************************ */

/************************************** File Number 2 :   HMAC-SHA1.C *******************************************************/

#define HMAC_SHA1_BITS        SHA1_HASH_BITS
#define HMAC_SHA1_BYTES       SHA1_HASH_BYTES
#define HMAC_SHA1_BLOCK_BITS  SHA1_BLOCK_BITS
#define HMAC_SHA1_BLOCK_BYTES SHA1_BLOCK_BYTES

typedef struct{
  sha1_ctx_t a; 
  sha1_ctx_t b;
} hmac_sha1_ctx_t;

#define IPAD 0x36
#define OPAD 0x5C

#define SECURE_WIPE_BUFFER

void hmac_sha1_init(hmac_sha1_ctx_t *s, const void *key, uint16_t keylength) // length in bits
        /*@ requires state_pred(&(s->a),_,_,_,_,_,_,_)  &*& state_pred(&(s->b),_,_,_,_,_,_,_) 
         &*& [?f]uchars(key, ?n_bytes, _) &*& 0 <= keylength &*& keylength <= n_bytes*8 &*& n_bytes <= 64; @*/

        /*@ ensures  state_pred(&(s->a),_,_,_,_,_,_,_) &*& state_pred(&(s->b),_,_,_,_,_,_,_) &*& [f]uchars(key, n_bytes, _); @*/
        
      
{
    
    uint8_t buffer[SHA1_BLOCK_BYTES];
    uint8_t i;
    
    memset(buffer, 0, SHA1_BLOCK_BYTES);
    //@ div_rem(keylength+7,8);
    uint16_t c = (uint16_t)((keylength+7)/8);
    //@ div_rem(keylength+7,8);
    //@ chars_to_uchars(buffer);
    //@ uchars_split(buffer,c);
    //@ uchars_to_chars(buffer);
    //@ uchars_split(key,c);
    //@ uchars_to_chars(key);
    memcpy(buffer, key, c); //memcpy_boot
    //@ chars_to_uchars(buffer);
    //@ uchars_join(buffer);
    //@ chars_to_uchars(key);
    //@ uchars_join(key);

    for (i=0; i<SHA1_BLOCK_BYTES; ++i)
        //@ invariant i >= 0 &*& uchars(buffer,SHA1_BLOCK_BYTES,_);
    {
    	uint8_t buffer_i = buffer[i];
    	//@ produce_limits(buffer_i);
    	//@ XOR_NoUnderflow_char(buffer[i], IPAD);
    	//@ XOR_NoOverflow_char(buffer[i], IPAD);
        buffer[i]  = /*@ truncating @*/(uint8_t)(buffer_i ^ IPAD);
    }
    sha1_init(&(s->a));
    //@ uchars_to_integers_(buffer,4,false,16);
    //@ div_rem(64,4);
    sha1_nextBlock(&(s->a), buffer);
    //@ integers__to_uchars(buffer); //uints_to_uchars(buffer);
    for (i=0; i<SHA1_BLOCK_BYTES; ++i)
        //@ invariant i >= 0 &*& uchars(buffer,SHA1_BLOCK_BYTES,_);
    {
        uint8_t buffer_i = buffer[i];
        //@ produce_limits(buffer_i);
    	//@ XOR_NoUnderflow_char(IPAD, OPAD);
    	//@ XOR_NoOverflow_char(IPAD, OPAD);
    	//@ XOR_NoUnderflow_char(buffer[i], IPAD^OPAD);
        buffer[i]  = /*@ truncating @*/(uint8_t)(buffer_i^IPAD^OPAD);
    }
    sha1_init(&(s->b));
    //@ uchars_to_integers_(buffer,4,false,16);
    sha1_nextBlock(&(s->a), buffer);
    
    
#if defined SECURE_WIPE_BUFFER
    memset(buffer, 0, SHA1_BLOCK_BYTES);
#endif
}

void hmac_sha1_nextBlock(hmac_sha1_ctx_t *s, const void *block)
        //@ requires state_pred(&(s->a),_,_,_,_,_,_,_) &*& [?f]((uint32_t *)block)[..16] |-> _;
        //@ ensures  state_pred(&(s->a),_,_,_,_,_,_,_) &*& [f]((uint32_t *)block)[..16] |-> _;
{   
    sha1_nextBlock(&(s->a), block);  
}


void hmac_sha1_lastBlock(hmac_sha1_ctx_t *s, const void *block, uint16_t length) //length in bits
        /*@ requires  state_pred(&(s->a),_,_,_,_,_,_,_) &*& [?f]uchars(block, ?n_bytes, _) &*& 0 <= length 
                        &*& length <= n_bytes*8 &*& n_bytes <= 256; @*/
        //@ ensures   state_pred(&(s->a),_,_,_,_,_,_,_) &*& [f]uchars(block, n_bytes, _);
{
    //@ void* block0 = block;
    //@ int bytesDone = 0;
    while(length>=SHA1_BLOCK_BITS)
        //@ invariant  state_pred(&(s->a),_,_,_,_,_,_,_) &*& [f]uchars(block0, n_bytes, _) &*& 0 <= bytesDone &*& block == block0 + bytesDone &*& 0 <= length &*& length <= (n_bytes - bytesDone) * 8;
    {
        //@ uchars_split(block0, bytesDone);
        //@ uchars_split(block, SHA1_BLOCK_BYTES);
        //@ uchars_to_integers_(block,4,false,16); // uchars_to_uints(block);
        //@ div_rem(SHA1_BLOCK_BYTES, 4);
        sha1_nextBlock(&(s->a), block);
        //@ integers__to_uchars(block);// uints_to_uchars(block);

        block = (uint8_t*)block + SHA1_BLOCK_BYTES;
        length -= SHA1_BLOCK_BITS;
        //@ uchars_join(block0);
        //@ bytesDone += 64;
    }
    //@ uchars_split(block0, block - block0);
    sha1_lastBlock(&(s->a), block, length);

}

void hmac_sha1_final(void *dest, hmac_sha1_ctx_t *s)
    /*@ 
        requires state_pred(&(s->a),_,_,_,_,_,_,_) &*& ((uint32_t *)dest)[..16] |-> _ &*& state_pred(&(s->b),_,_,_,_,_,_,_); 
    @*/
    /*@
        ensures uchars(dest,16*4,_) &*& state_pred(&(s->a),_,_,_,_,_,_,_) &*& state_pred(&(s->b),_,_,_,_,_,_,_); 
    @*/ 
{
    
    //@ integers__to_uchars(dest); //uints_to_uchars(dest);
    //@ uchars_split(dest,20);
    //@ uchars_to_integers_(dest,4,false,5);//uchars_to_uints(dest);
    //@ div_rem(20,4);
    sha1_ctx2hash(dest, &(s->a));
    //@ integers__to_uchars(dest); //uints_to_uchars(dest);
    //@ uchars_join(dest);
    sha1_lastBlock(&(s->b), dest, SHA1_HASH_BITS);
    //@ uchars_split(dest,20);
    //@ uchars_to_integers_(dest,4,false,5); //uchars_to_uints(dest);
    //@ div_rem(20,4);
    sha1_ctx2hash(dest, &(s->b));
    //@ integers__to_uchars(dest); //uints_to_uchars(dest);
    //@ uchars_join(dest);
}

/* ************************************************************************************************************************ */

/************************************** File Number 3 :   microvisor.C *******************************************************/

#define METADATA_OFFSET APP_META
#define PAGE_SIZE 256



const uint16_t uvisor_entrypoints[] = {                    
    0x0001, //(uint16_t) &safe_icall_ijmp,
    0x0002, //(uint16_t) &safe_ret,
    0x0003, //(uint16_t) &safe_reti,
    (uint16_t) &load_image,
    (uint16_t) &verify_activate_image,
    (uint16_t) &remote_attestation,
    0x0000
};

/****************************************************************************/
/*                      MICROVISOR HELPER FUNCTIONS                         */
/****************************************************************************/

/* Verifies if _WORD_ address is safe to jump to for the app image to be
 * deployed */
static uint8_t verify_target_deploy(uint16_t target) 
    //@ requires true; // 0 <= target;
    //@ ensures 0 <= result &*& result <= 1;
{
  uint16_t address;
  uint16_t addr_ptr;
  uint8_t i;
  uint8_t RAMPZ; // temporary defintion to avoid errors in formal verification
  uint8_t result;

  /* Everything is in upper progemem: 17th bit should be 1 for all these reads
   * */
  RAMPZ = 0x01;

  /* Get .data byte address (= end of app .text) and convert it to a word
   * address */
  address = pgm_read_word_far_no_rampz((uint32_t)(SHADOW_META + 2));
  address = /*@ truncating @*/(uint16_t)(address >> 1);
  if(target >= address) { /* Target is not inside app .text */
    /* Init pointer to entrypoint array in progmem */
    void *uvisor_entrypoints_ptr = &uvisor_entrypoints;
    //@ produce_limits(uvisor_entrypoints_ptr);
    
    addr_ptr = (uint16_t) &uvisor_entrypoints;
    do 
        //@invariant true;
    {
      address = pgm_read_word_far_no_rampz(addr_ptr);
      if(target == address){
        result = 1;      /* Success, target is entrypoint to uvisor */
        return result;
      }
      //@ assume_ADD_NoOverflow_2chars(addr_ptr,2);
      addr_ptr += 2;
    } while(address != 0x0000);

    /* Target outside of app .text + not an uvisor entrypoint */
    result = 0;
    return result;
  } else { /* Target is inside app .text */
    uint16_t i_ = pgm_read_word_far_no_rampz((uint32_t)(SHADOW_META + 4));
    i = /*@truncating@*/ (uint8_t)i_;
    uint16_t numBytes = 6;
    addr_ptr = /**((uint16_t) SHADOW_META) +**/ numBytes;
    while(i > 0)
        //@invariant true;
     {
      address = pgm_read_word_far_no_rampz(addr_ptr);
      if(target == address)
        {        
            result = 0;
            return result; /* Fail, target is unsafe 2nd word */
        }
    //@ assume_ADD_NoOverflow_2chars(addr_ptr,2);     
    addr_ptr += 2;
      i--;
    }

    /* Target inside of app .text + not an unsafe 2nd word --> success */
    result = 1;
    return result;
  }
}

/* Writes to arbitrary page of progmem */
static void
write_page(uint8_t *page_buf, uint32_t offset) 
    //@ requires [?f]uchars(page_buf, 256, _) &*& 0 <= offset &*& offset <= UINT_MAX;
    //@ ensures [f]uchars(page_buf, 256, _);
{
  uint32_t pageptr;
  uint8_t i;
  /* Erase page */
  boot_page_erase(offset);
  boot_spm_busy_wait();

  /* Write a word (2 bytes) at a time */
  pageptr = offset;
   //@ div_rem(PAGE_SIZE, 2);
  i = (uint8_t)(PAGE_SIZE/2);
  //@ div_rem(PAGE_SIZE, 2);
  
  //@ uint8_t *page_buf0 = page_buf;
  //@ uint8_t offsetsDone = 0;
  
  while (i > 0)
        /*@ invariant [f]uchars(page_buf0, 256, _)  &*& 0 <= i &*& 0 <= offsetsDone &*& 0 <= pageptr &*& pageptr <= UINT_MAX
        &*& offsetsDone <= 256 &*& page_buf == page_buf0 + offsetsDone &*& i <= (256 - offsetsDone) ; @*/

  {
      //@ uchars_split(page_buf0,offsetsDone);
      uint16_t w = *page_buf; 
      page_buf++;
      
      //@ assume(offsetsDone+1 <= 256-offsetsDone-1);
      //@ uchars_split(page_buf0+offsetsDone+1,offsetsDone+1);
      //@ assume( 0 <= *page_buf  && *page_buf <= 255);
      //@ shiftLeftNoUnderflow(*page_buf , 8);
      //@ assume_shiftLeftNoOverflow_1char(*page_buf , 8);
      uint8_t tempW = (uint8_t)((*page_buf) << 8);
      //@ assume ( 0 <= w && w <= UINT16_MAX);
      //@ OR_NoUnderflow(w, tempW);
      //@ OR_NoOverflow_2chars(w , tempW);
      w |= tempW;
      page_buf++;
     
      boot_page_fill(pageptr, w);
      //@ assume_ADD_NoOverflow_2chars(pageptr,2);
      pageptr += 2;
      i--;
      //@ offsetsDone += 2;
      //@ close [f]uchars(page_buf-2,256-offsetsDone+2,_);
      //@ uchars_join(page_buf0);
      //@ assume ( i <= (256 - offsetsDone)); //it should be implicitly understood by verifast!

  }

  boot_page_write(offset);     // Store buffer in flash page.
  boot_spm_busy_wait();        // Wait until the memory is written.

  /* Reenable RWW-section again. We need this if we want to jump back to the
   * application after bootloading. */
  boot_rww_enable();
}

void read_page(uint8_t *page_buf, uint32_t offset) 
    //@ requires uchars(page_buf, PAGE_SIZE, _) &*& 0 <= offset;
    //@ ensures uchars(page_buf, PAGE_SIZE, _);
{

    /* Assembly code */
      /*
  uint8_t _reg = (uint8_t) (offset >> 16);
  uint16_t _off = (uint16_t) offset;
  __asm__ __volatile__ (
      "out %6, %0\n\t"
      "clr %0\n\t"
      "1: elpm __tmp_reg__, Z+\n\t"
      "st %a2+, __tmp_reg__\n\t"
      "inc %0\n\t"
      "brne 1b\n\t"
      : "=r" (_reg), "=z" (_off), "=e" (page_buf)
      : "0" (_reg), "1" (_off), "2" (page_buf), "I" (_SFR_IO_ADDR(RAMPZ))
  );
  */
}

void load_key(uint8_t *buff) 
    //@ requires uchars(buff,20,_);
    //@ ensures uchars(buff,20,_);
{
    
    /* Assembly code */
    /*
  uint8_t _reg;
  uint8_t *_buff = buff;
  uint8_t *_key = (uint8_t *) key_hmac;
  __asm__ __volatile__ (
      "ldi %0, 0x01\n\t"
      "out %5, %0\n\t"
      "ldi %0, 20\n\t"
      "1: elpm __tmp_reg__, Z+\n\t"
      "st %a2+, __tmp_reg__\n\t"
      "dec %0\n\t"
      "brne 1b\n\t"
      : "=&d" (_reg), "=z" (_key), "=e" (_buff)
      : "1" (_key), "2" (_buff), "I" (_SFR_IO_ADDR(RAMPZ))
  );
  */

}

/* Activates image by transfering image from deploy to running app space */
void switch_image() 
    //@ requires true;
    //@ ensures true;
{
  uint16_t pages;
  uint16_t i;
  uint8_t buf[PAGE_SIZE];

  /* Calculate amount of pages to copy */
  //RAMPZ = 0x01;
  pages = pgm_read_word_far_no_rampz(SHADOW_META);
  uint16_t temp_page = pages;
  //@ div_rem(pages, PAGE_SIZE);
  pages = (uint16_t)(pages/PAGE_SIZE);

  //@ div_rem(temp_page, PAGE_SIZE);
  if (temp_page%PAGE_SIZE > 0)
    pages++;

  for(i=0; i<pages; i++)
  //@ invariant i >= 0 &*& uchars(buf, 256,_); 
  {  
    read_page(buf, ((uint32_t) SHADOW) + (uint32_t)PAGE_SIZE*i);
    write_page(buf, (uint32_t)PAGE_SIZE*i);
  }

  /* Copy metadata page */
  read_page(buf, SHADOW_META);
  write_page(buf, APP_META);
}

uint8_t verify_shadow() 
    //@ requires true;
    //@ ensures true;
{
  uint16_t current_word;
  uint16_t current_addr; //In WORDS, not bytes
  uint16_t pointer;
  uint8_t pointer_rampz;
  uint16_t text_size; //In WORDS, not bytes
  uint8_t prev_op_long;
  uint8_t tempBool;


  /* Init text_size variable */
  //RAMPZ = 0x01;
  text_size = pgm_read_word_far_no_rampz((uint32_t)(SHADOW_META + 2));
  text_size = /*@ truncating @*/(uint16_t)(text_size >> 1);

  /* Loop over instructions word by word */
  pointer_rampz = 0x00;
  pointer = SHADOW;
  current_addr = 0x0000;
  prev_op_long = 0;
  while(current_addr < text_size) 
    //@ invariant true;
  {
    /* Fetch next word */
    //RAMPZ = pointer_rampz;
    current_word = pgm_read_word_far_no_rampz(pointer);

    /* Check target address of previous long op is correct */
    tempBool = verify_target_deploy(current_word);
    if(prev_op_long && !tempBool)
      return 0;

    /* Parse word as instruction and check if it is allowed. If this word is a
     * long call address (prev_op_long == 1), check if absent from list (i.e.
     * verify_target says we CAN jump to it) before rejecting */
    if((current_word == 0x940C) || (current_word == 0x940E)) {
      /* ---LONG INSTRUCTIONS WITH TARGET AS 2ND WORD--- */
      /* If target addr of long call gets decoded as long call and is not on
       * list, reject */
       tempBool == verify_target_deploy(current_addr);
        if(prev_op_long && tempBool)
            return 0;
      /* Set prev_op_long flag correctly: 1 in the normal case, and 0 if this
       * is the address of a long call. */
      //prev_op_long = !prev_op_long;
      prev_op_long = /*@ truncating @*/ (uint8_t) (prev_op_long == 1 ? 0 : 1) ;
    } else {
      /* ---NORMAL INSTRUCTIONS--- */

      /* Ops which need a relative address calculated set this flag. Signed
       * offset is stored in current_word */
      uint8_t calc_rel = 0;
      if((current_word == 0x9508)
          || (current_word == 0x9518)
          || (current_word == 0x9409)
          || (current_word == 0x9509)
          || (current_word == 0x95D8)
          || (current_word & 0xFE0F) == 0x9006
          || (current_word & 0xFE0F) == 0x9007) {
        /* PLAIN UNSAFE OPS: RET, RETI, IJMP, ICALL, ELPM, ELPM RD,Z(+) */
        /* In normal situation, reject. As target address of long call, reject
         * if not on list. */
        tempBool = verify_target_deploy(current_addr);
        if( !prev_op_long || (prev_op_long && tempBool) )
          return 0;
      } else if((current_word & 0xFC00) == 0xF000
          || (current_word & 0xFC00) == 0xF400) {
        /* BRANCH OPS: extract offset and make signed 16 bit int */
        current_word &= 0x03F8;
        //@shiftRightNoUnderflow(current_word,3);
        //@shiftRight16NoOverflow(current_word,3);
        current_word >>= 3;
        if(current_word > 0x3F){
            //@ OR_NoUnderflow(current_word, 0xFF80);
            //@ OR_NoOverflow_2chars(current_word, 0xFF80);
          current_word |= 0xFF80;
      }
        calc_rel = 1;
      } else if((current_word & 0xF000) == 0xC000
          || (current_word & 0xF000) == 0xD000) {
        /* RJMP OR RCALL: extract offset and make signed 16 bit int */
        current_word &= 0x0FFF;
        if(current_word > 0x07FF){
        //@ OR_NoUnderflow(current_word, 0xF000);
        //@ OR_NoOverflow_2chars(current_word, 0xF000);
          current_word |= 0xF000;
      }
        calc_rel = 1;
      }

      /* Calculate target and check it */
      if(calc_rel) {
        //@ assume_ADD_NoOverflow_2chars(current_addr, current_word);
        current_word = (uint16_t)(current_addr +  current_word);
        //@ assume_ADD_NoOverflow_2chars_plusplus(current_word, 1);
        current_word++;
        if(!verify_target_deploy(current_word)) {
          /* In normal situation, reject. As target address of long call,
           * reject if not on list. */
            tempBool = verify_target_deploy(current_addr);
          if( !prev_op_long || (prev_op_long && tempBool) )
            return 0;
        }
      }

      /* Set prev_op_long flag accordingly */
      prev_op_long = 0;
    }

    /* Increment loop variables */
    //@ assume_ADD_NoOverflow_2chars(current_addr,1);
    current_addr++;
    //@ assume_ADD_NoOverflow_2chars(pointer,2);
    pointer +=2;
    if(pointer == 0x0000)
      pointer_rampz = 0x01;
  }

  return 1;
}

/*@
    lemma void ensure_state_pred (hmac_sha1_ctx_t* ctx)
        requires integers_((&(ctx->a))->h,4,false, 5, _) &*& integers_((&(ctx->b))->h,4,false, 5, _) &*& sha1_ctx_t_length(&(ctx->a), ?L1) &*& sha1_ctx_t_length(&(ctx->b), ?L2);
        ensures state_pred(&(ctx->a),_,_,_,_,_,_,L1)  &*& state_pred(&(ctx->b),_,_,_,_,_,_,L2);
    {
        close state_pred(&(ctx->a),_,_,_,_,_,_,L1);
        close state_pred(&(ctx->b),_,_,_,_,_,_,L2);
    }

@*/

uint8_t verify_hmac() 
    //@ requires true;
    //@ ensures 0 <= result &*& result <= 1;
{
  uint16_t image_size; //in BYTES
  uint16_t meta_size = 3; //in BYTES, without digest
  uint8_t digest[20]; //HMAC_SHA1_BYTES
  uint8_t result;

  uint32_t offset = SHADOW;
  hmac_sha1_ctx_t ctx;
  uint8_t buff[PAGE_SIZE];

  /* Init image_size variable */
  //RAMPZ = 0x01;
  image_size = pgm_read_word_far_no_rampz(SHADOW_META);
  uint32_t temp_SHADOW_META = (uint32_t)(SHADOW_META + 4);
  uint16_t temp_meta = pgm_read_word_far_no_rampz(temp_SHADOW_META);
  //@ assume_ADD_NoOverflow_2chars(meta_size, temp_meta);
  meta_size  = meta_size + temp_meta;
  //@ assume_shiftLeftNoOverflow_2chars(meta_size,1);
  meta_size <<= 1; //Convert words to bytes

  /* Init hmac context with key (load 20 byte key temporary in buff) */

  //@ uchars_split(buff, 20);
  load_key(buff);
  //@ ensure_state_pred (&ctx);
  hmac_sha1_init(&ctx, buff, 160);
  //@ uchars_join(buff);
  
  image_size = compute_hmac_block(&ctx , image_size, buff, offset);

  
  /* Hash last (semi)page + metadata */
  read_page(buff, offset);
 
  hmac_sha1_lastBlock(&ctx, buff, (uint16_t)(image_size*8));
  //@ uchars_to_chars(buff);
  //@ assume(image_size+meta_size+20 < 256);
  //@ chars_split(buff, (uint8_t)(image_size+meta_size+20));

  void* buff_p = getAddressBuff(buff,  (uint8_t)image_size,  (uint8_t)meta_size);
  memcpy(digest, (char *)buff_p, 20); //Backup digest from metadata page //memcpy_boot
  //@ chars_join(buff);
  //@ chars_join(buff);
  //@ chars_to_uchars(buff);
  //@ uchars_split(buff,64);
  //@ div_rem(64,4);
  /* Finalize + compare */
  hmac_sha1_final(buff, &ctx);
  //@ uchars_split(buff,20);
  //@ uchars_to_chars(buff);
  if(memcmp(buff, digest, 20) != 0) //memcpy_boot
    result =  0;
  else
   result =  1;
  //@ chars_to_uchars(buff);
  //@ uchars_join(buff);
  //@ uchars_join(buff);

return result;
//@ open state_pred(&((&ctx)->a),_,_,_,_,_,_,_);
//@ open state_pred(&((&ctx)->b),_,_,_,_,_,_,_);

}

void* getAddressBuff( int8_t* buff, uint8_t par1, uint8_t par2)
	//@ requires chars(buff,20+par1+par2,_);
	//@ ensures chars(buff,par1+par2,_) &*& chars(buff+par1+par2,20,_) &*& result == buff+par1+par2;
{
	 //@ chars_limits(buff);
	//@ chars_split(buff,par1+par2);
	
	void* result = buff+par1+par2;
	return result;
}

uint16_t compute_hmac_block(hmac_sha1_ctx_t* ctx , uint16_t image_size, uint8_t* buff, uint32_t offset)
    //@ requires uchars(buff,PAGE_SIZE,_) &*& state_pred(&(ctx->a),_,_,_,_,_,_,_) &*&  0 <= offset &*& offset <= UINT_MAX &*& 0 <= image_size;
    //@ ensures uchars(buff,PAGE_SIZE,_)  &*& state_pred(&(ctx->a),_,_,_,_,_,_,_) &*&  0 <= result &*& result <= PAGE_SIZE;
{
    uint16_t result = 0;
      /* Hash full app pages first */
  while(image_size >= PAGE_SIZE) 
    //@ requires uchars(buff,PAGE_SIZE,_)  &*& state_pred(&(ctx->a),_,_,_,_,_,_,_) &*&  0 <= offset &*& offset <= UINT_MAX;
    //@ ensures uchars(buff,PAGE_SIZE,_)  &*& state_pred(&(ctx->a),_,_,_,_,_,_,_) &*& result <= PAGE_SIZE;
  {
    read_page(buff, offset);
    /* Hash full page, unroll loop */

    //@ uchars_split(buff, 64);
    //@ div_rem(64,4);
    hmac_sha1_nextBlock(ctx, buff);
    //@ uchars_split(buff+64, 64);
    //@ div_rem(64,4);
    hmac_sha1_nextBlock(ctx, buff + HMAC_SHA1_BLOCK_BYTES);
    //@ uchars_split(buff+128, 64);
    //@ div_rem(64,4);
    hmac_sha1_nextBlock(ctx, buff + HMAC_SHA1_BLOCK_BYTES*2);
    //@ uchars_split(buff+192, 64);
    //@ div_rem(64,4);
    hmac_sha1_nextBlock(ctx, buff + HMAC_SHA1_BLOCK_BYTES*3);
    /* Book keeping */
    image_size -= PAGE_SIZE;
    //@ assume_ADD_NoOverflow_offset_pagesize(offset, PAGE_SIZE);
    offset += PAGE_SIZE;

    //@ uints_to_uchars(buff+192);
    //@ uchars_join(buff+192);
    //@ uints_to_uchars(buff+128);
    //@ uchars_join(buff+128);
    //@ uints_to_uchars(buff+64);
    //@ uchars_join(buff+64);
    //@ uints_to_uchars(buff);
    //@ uchars_join(buff);
  }
    result = image_size;
  return result;
}

void load_image(uint8_t *page_buf, uint16_t offset) 
    //@ requires uchars(page_buf, 256, _) &*& 0 <= offset;
    //@ ensures uchars(page_buf, 256, _);
{
  //uint8_t sreg;
  //sreg = SREG;
  cli();

  /* Write page if it is within the allowable space */
  if(offset<SHADOW)
    write_page(page_buf, offset); //shadow

  //SREG = sreg;
}

uint8_t verify_activate_image() 
    //@ requires true;
    //@ ensures true;
{
  //uint8_t sreg;
  //sreg = SREG;
  cli();

  uint8_t r1 = verify_shadow();
  uint8_t r2 = verify_hmac();
  if(!r1 || !r2)
    return 0;

  /* We passed all tests, activate new image and jump to it */
  switch_image();
  //SREG = sreg;
  //goto *(0x0000);
  return 1;
}

void remote_attestation(uint8_t *mac) 
    //@ requires uchars(mac,64,_); // 20 bytes nonce + 44 bytes random data
    //@ ensures uchars(mac,64,_); // 20 bytes MAC value + 44 bytes random data
{
  //uint8_t sreg;
  //sreg = SREG;
  //cli();

  hmac_sha1_ctx_t ctx;
  uint8_t buff[PAGE_SIZE];

  /* Init hmac context with key (load 20 byte key temporary in buff) */
  
  load_key(buff);
  //@ ensure_state_pred (&ctx);
  hmac_sha1_init(&ctx, buff, 160);
  //@ uchars_join(buff);

  /* Hash full image */
  compute_hash_memory (&ctx , buff);

  /* Hash nonce */
  //@ uchars_split(mac, 20);
  hmac_sha1_lastBlock(&ctx, mac, 160); //20 byte nonce: 20*8
  //@ uchars_join(mac);

  /* Finalize */
  //@ div_rem(64,4);
  hmac_sha1_final(mac, &ctx);

  //SREG = sreg;

  //@ open state_pred(&((&ctx)->a),_,_,_,_,_,_,_);
  //@ open state_pred(&((&ctx)->b),_,_,_,_,_,_,_);
}

void compute_hash_memory (hmac_sha1_ctx_t* ctx , uint8_t* buff)
    //@ requires state_pred(&(ctx->a),_,_,_,_,_,_,_) &*& uchars(buff,PAGE_SIZE,_);
    //@ ensures state_pred(&(ctx->a),_,_,_,_,_,_,_) &*& uchars(buff,PAGE_SIZE,_);

{

    /* Hash full image */
   uint32_t offset = 0x00;
  while(offset < MEM_END) 

        //@ invariant state_pred(&(ctx->a),_,_,_,_,_,_,_) &*& uchars(buff,PAGE_SIZE,_) &*& 0 <= offset;
  {
    read_page(buff, offset);

    /* Run block through HMAC, unroll loop */
    //@ uchars_split(buff, 64);
    //@ div_rem(64,4);
    hmac_sha1_nextBlock(ctx, buff);
    //@ uchars_split(buff+64, 64);
    //@ div_rem(64,4);
    hmac_sha1_nextBlock(ctx, buff + HMAC_SHA1_BLOCK_BYTES);
    //@ uchars_split(buff+128, 64);
    //@ div_rem(64,4);
    hmac_sha1_nextBlock(ctx, buff + HMAC_SHA1_BLOCK_BYTES*2);
    //@ uchars_split(buff+192, 64);
    //@ div_rem(64,4);
    hmac_sha1_nextBlock(ctx, buff + HMAC_SHA1_BLOCK_BYTES*3);
    /* Book keeping */
    offset += PAGE_SIZE; //SPM_PAGESIZE

    //@ integers__to_uchars(buff+192);
    //@ uchars_join(buff+192);
    //@ integers__to_uchars(buff+128);
    //@ uchars_join(buff+128);
    //@ integers__to_uchars(buff+64);
    //@ uchars_join(buff+64);
    //@ integers__to_uchars(buff);
    //@ uchars_join(buff);
   
  }
}


