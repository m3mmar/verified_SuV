# Ugly hack that overrides udivmodsi4 (32bit/32bit integer division) in library
# to use safe_ret. Normally the toolchain would be recompiled with subsitutions
# already in place.

    .section .text.libgcc.div, "ax", @progbits
    .global __udivmodsi4
__udivmodsi4:
    ldi r26, 33 ; init loop counter
    mov r1, r26
    sub r26,r26
    sub r27,r27 ; clear remainder and carry
    movw r30, r26
    rjmp __udivmodsi4_ep ; jump to entry point
__udivmodsi4_loop:
    rol r26 ; shift dividend into remainder
    rol r27
    rol r30
    rol r31
    cp r26,r18 ; compare remainder & divisor
    cpc r27,r19
    cpc r30,r20
    cpc r31,r21
    brcs __udivmodsi4_ep ; remainder <= divisor
    sub r26,r18 ; restore remainder
    sbc r27,r19
    sbc r30,r20
    sbc r31,r21
__udivmodsi4_ep:
    rol r22 ; shift dividend (with CARRY)
    rol r23
    rol r24
    rol r25
    dec r1 ; decrement loop counter
    brne __udivmodsi4_loop
    ; r1 now restored (r1 == 0)
    com r22
    com r23
    com r24
    com r25
; div/mod results to return registers, as for the ldiv() function
    movw r18, r22 ; quotient
    movw r20, r24
    movw r22, r26 ; remainder
    movw r24, r30
    jmp safe_ret
