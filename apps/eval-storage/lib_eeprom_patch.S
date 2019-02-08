# Ugly hack that overrides eeprom library functions. Normally the toolchain
# would be recompiled with subsitutions already in place.

    .section .text.libgcc.div, "ax", @progbits
    .global eeprom_read_block
eeprom_read_block:
    movw    r26, r24
    movw    r24, r22

eeprom_read_blraw:
    movw    r30, r24
    sbic    0x1f, 1 ; 31
    rjmp    .-4             ; 0x1c2 <eeprom_read_blraw+0x2>
    rjmp    .+12            ; 0x1d4 <eeprom_read_blraw+0x14>
    out     0x22, r31       ; 34
    out     0x21, r30       ; 33
    sbi     0x1f, 0 ; 31
    adiw    r30, 0x01       ; 1
    in      r0, 0x20        ; 32
    st      X+, r0
    subi    r20, 0x01       ; 1
    sbci    r21, 0x00       ; 0
    brcc    .-18            ; 0x1c8 <eeprom_read_blraw+0x8>
    jmp     safe_ret

    .section .text.libgcc.div, "ax", @progbits
    .global eeprom_write_block
eeprom_write_block:
    movw    r26, r24
    movw    r24, r22
    rjmp    .+6             ; 0x1e8 <eeprom_write_block+0xc>
    ld      r18, X+
    call    eeprom_write_r18
    subi    r20, 0x01       ; 1
    sbci    r21, 0x00       ; 0
    brcc    .-12            ; 0x1e2 <eeprom_write_block+0x6>
    jmp     safe_ret

eeprom_write_byte:
    mov     r18, r22

eeprom_write_r18:
    sbic    0x1f, 1         ; Loop until EEPE bit in EECR is clear
    rjmp    eeprom_write_r18
    out     0x1f, r1        ; EECR reg cleared
    out     0x22, r25       ; EEARH address reg
    out     0x21, r24       ; EEARL address reg
    out     0x20, r18       ; EEDR data reg
    in      r0, 0x3f        ; Save SREG
    cli                     ; Disable global interrupts
    sbi     0x1f, 2         ; Set EEMPE in EECR
    sbi     0x1f, 1         ; Set EEPE in EECR
    out     0x3f, r0        ; Restore SREG
    adiw    r24, 0x01       ; Increment addr with 1
    jmp     safe_ret
