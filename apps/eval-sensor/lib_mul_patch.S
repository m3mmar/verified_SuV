# Ugly hack that overrides usmulhisi3 (multi-byte integer multiplication) in
# library to use safe_ret. Normally the toolchain would be recompiled with
# subsitutions already in place.

    .section .text.libgcc.div, "ax", @progbits
    .global __usmulhisi3
__usmulhisi3:
    call    __umulhisi3
__usmulhisi3_tail:
    sbrs    r27, 7
    jmp     safe_ret
    sub     r24, r18
    sbc     r25, r19
    jmp safe_ret
__umulhisi3:
    mul     r26, r18
    movw    r22, r0
    mul     r27, r19
    movw    r24, r0
    mul     r26, r19
    add     r23, r0
    adc     r24, r1
    eor     r1, r1
    adc     r25, r1
    mul     r27, r18
    add     r23, r0
    adc     r24, r1
    eor     r1, r1
    adc     r25, r1
    jmp     safe_ret
