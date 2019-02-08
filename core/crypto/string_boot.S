#define dest_hi r25
#define dest_lo r24
#define src_hi r23
#define src_lo r22
#define len_hi r21
#define len_lo r20

	.section .bootloader,"ax",@progbits

    .global    memcpy_boot
    .type    memcpy_boot, @function
memcpy_boot:
    movw    r30, src_lo
    movw    r26, dest_lo
    rjmp    .L_memcpy_start
.L_memcpy_loop:
    ld    r0, Z+
    st    X+, r0
.L_memcpy_start:
    subi    len_lo, lo8(1)
    sbci    len_hi, hi8(1)
    brcc    .L_memcpy_loop
    ret
.L_memcpy_end:
    .size    memcpy_boot, .L_memcpy_end - memcpy_boot


#define s1_hi r25
#define s1_lo r24
#define s2_hi r23
#define s2_lo r22

#define ret_hi r25
#define ret_lo r24

	    .section .bootloader,"ax",@progbits
        .global memcmp_boot
        .type   memcmp_boot, @function
memcmp_boot:
        movw  r30, s2_lo
        movw  r26, s1_lo
        rjmp    .L_memcmp_start
.L_memcmp_loop:
        ld      ret_lo, X+
        ld      r0, Z+
        sub     ret_lo, r0
        brne    .L_memcmp_done
.L_memcmp_start:
        subi    len_lo, lo8(1)
        sbci    len_hi, hi8(1)
        brcc    .L_memcmp_loop
; strings are equal, so clear both ret_lo and carry
        sub     ret_lo, ret_lo
.L_memcmp_done:
; ret_hi = SREG.C ? 0xFF : 0
        sbc     ret_hi, ret_hi
        ret
.L_memcmp_end:
        .size   memcmp_boot, .L_memcmp_end - memcmp_boot
