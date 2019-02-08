#include "mem_layout.h"

__SP_H__ = 0x3e
__SP_L__ = 0x3d
__SREG__ = 0x3f
__RAMPZ__ = 0x3b
__tmp_reg__ = 0
__zero_reg__ = 1
    .section .bootloader,"ax",@progbits

/*********************************/
/* CONTROL TRANSFER INSTRUCTIONS */
/*********************************/


/* Combined for ijmp and icall
 * ijmp  --> jmp safe_icall_ijmp (no RA on stack)
 * icall --> call safe_icall_ijmp (RA of original caller on stack) */
.global safe_icall_ijmp
    .type safe_icall_ijmp, @function
safe_icall_ijmp:
    /* Back up SREG + disable global interrupts. Use r18 since it is a clobber anyway */
    push r18
    in r18,__SREG__
    cli
    push r18
    /* Backup RAMPZ */
    in r18,__RAMPZ__
    push r18
    /* Backup remaining clobbers */
    push r19
    push r20
    push r24
    push r25
    push r30
    push r31
    /* CHECK Z reg */
    movw r24,r30
    rcall verify_target_running
    tst r24
    breq test_fail
    /* Restore clobbers, except r18 */
    pop r31
    pop r30
    pop r25
    pop r24
    pop r20
    pop r19
    /* Restore RAMPZ */
    pop r18
    out __RAMPZ__,r18
    /* Restore SREG + r18. If global interrupts were enabled, they are reenabled now */
    pop r18
    out __SREG__,r18
    pop r18
    /* Carry out jump */
    ijmp
	.size	safe_icall_ijmp, .-safe_icall_ijmp

/* For now, just hang on illegal operation */
test_fail:
    rjmp .-2

/* ret --> jmp safe_ret (only RA of callers caller on stack) */
.global safe_ret
    .type safe_ret, @function
safe_ret:
    /* Back up SREG + disable global interrupts. Use r18 since it is a clobber anyway */
    push r18
    in r18,__SREG__
    cli
    push r18
    /* Backup RAMPZ */
    in r18,__RAMPZ__
    push r18
    /* Backup remaining clobbers */
    push r19
    push r20
    push r24
    push r25
    push r30
    push r31
    /* Check RA which is at SP+0x0A and SP+0x0B */
    in r30,__SP_L__
    in r31,__SP_H__
    adiw r30,0x0A
    /* !!Caution!! AVRs RA is exceptionally stored big endian on stack */
    ld r25,Z+
    ld r24,Z
    rcall verify_target_running
    tst r24
    breq test_fail
    /* Restore clobbers, except r18 */
    pop r31
    pop r30
    pop r25
    pop r24
    pop r20
    pop r19
    /* Restore RAMPZ */
    pop r18
    out __RAMPZ__,r18
    /* Restore SREG + r18. If global interrupts were enabled, they are reenabled now */
    pop r18
    out __SREG__,r18
    pop r18
    /* Carry out return */
    ret
	.size	safe_ret, .-safe_ret

/* reti --> jmp safe_reti (only RA of callers caller on stack) */
.global safe_reti
    .type safe_reti, @function
safe_reti:
    /* Back up SREG + disable global interrupts. Use r18 since it is a clobber anyway */
    push r18
    in r18,__SREG__
    cli
    push r18
    /* Backup RAMPZ */
    in r18,__RAMPZ__
    push r18
    /* Backup remaining clobbers */
    push r19
    push r20
    push r24
    push r25
    push r30
    push r31
    /* Check RA which is at SP+0x0A and SP+0x0B */
    in r30,__SP_L__
    in r31,__SP_H__
    adiw r30,0x0A
    /* !!Caution!! AVRs RA is exceptionally stored big endian on stack */
    ld r25,Z+
    ld r24,Z
    rcall verify_target_running
    tst r24
    breq test_fail
    /* Restore clobbers, except r18 */
    pop r31
    pop r30
    pop r25
    pop r24
    pop r20
    pop r19
    /* Restore RAMPZ */
    pop r18
    out __RAMPZ__,r18
    /* Restore SREG + r18. If global interrupts were enabled, they are reenabled now */
    pop r18
    out __SREG__,r18
    pop r18
    /* Carry out return */
    reti
	.size	safe_reti, .-safe_reti

/* Checked statically by verifier: branch*, jmp/call, rjmp/rcall */

/*************************************/
/* TARGET ADDR CHECK HELPER FUNCTION */
/*************************************/

/* Checks target addr for running app:
 * (heavily optimised version of verify_target_deploy C function)
 * - Arg: r25:r24
 * - Ret: r24
 * - Clobber: r18,r19,r20 + Z (r31:r30) */
	.type	verify_target_running, @function
verify_target_running:
    /* Load .text end in bytes from APP_META+2 in progmem to r19:r18 */
	ldi r30,lo8(APP_META + 2)
	ldi r31,hi8(APP_META + 2)
	lpm r18, Z+
	lpm r19, Z
    /* Turn byte address into word address by bit shifting it right by 1 bit */
	lsr r19
	ror r18
    /* r19:r18 (end of APP .text) >= r25:r24 (argument, target address)? */
	cp r24,r18
	cpc r25,r19
	brlo .LTARGET_IN_TEXT
/***** Target is outside of app .text! ******/
    /* Load first 16 bits of uvisor_entrypoints addr into Z-pointer */
	ldi r30,lo8(uvisor_entrypoints)
	ldi r31,hi8(uvisor_entrypoints)
    /* Set 17th bit of Z-pointer high, uvisor_entrypoints sits in upper 64k */
	ldi r18,0x01
	out __RAMPZ__,r18
.LENTRY_LOOP:
    /* r19:r18 contains a legal entrypoint to uvisor */
	elpm r18, Z+
	elpm r19, Z+
    /* r19:r18 (legal entrypoint) == r25:r24 (argument, target address)? --> success */
	cp r24,r18
	cpc r25,r19
	brne .LCONT
.LEXIT_SUCCESS:
	ldi r24,0x01
	ret
.LCONT:
    /* Loop condition: test if uvisor_entrypoints[i] == 0x0000 */
	or r18,r19
	brne .LENTRY_LOOP
    /* Failure if loop ends without finding a valid entrypoint */
.LEXIT_FAILURE:
	ldi r24,0
	ret
/***** Target is in app .text! ******/
.LTARGET_IN_TEXT:
    /* Load num of bad 2nd words from APP_META+4 in progmem to r21:r20 */
	ldi r30,lo8(APP_META + 4)
	ldi r31,hi8(APP_META + 4)
	lpm r20, Z /* 8 least significant bits seem to be sufficient */
	#lpm r20, Z+
	#lpm r21, Z
    /* Set Z pointer to APP_META+6 (first element of 2nd bad words array) */
	ldi r30,lo8(APP_META + 6)
	ldi r31,hi8(APP_META + 6)
.LWORD_LOOP:
    /* Success if loop ends without finding illegal target match */
    tst r20
    breq .LEXIT_SUCCESS
    /* r19:r18 contains an illegal 2nd word to jump to in .text */
	lpm r18, Z+
	lpm r19, Z+
    /* r19:r18 (illegal word) == r25:r24 (argument, target address)? --> failure */
	cp r24,r18
	cpc r25,r19
	breq .LEXIT_FAILURE
    /* Decrement loop variable + check loop condition */
	subi r20,0x01
	rjmp .LWORD_LOOP
	.size	verify_target_running, .-verify_target_running

#dump_shit:
#    lds     r0, 0x00C8
#    sbrs    r0, 5
#    rjmp    .-8
#    sts     0x00CE, r24
#    lds     r0, 0x00C8
#    sbrs    r0, 5
#    rjmp    .-8
#    sts     0x00CE, r25
#    ret

###/****************************/
###/* DATA ACCESS INSTRUCTIONS */
###/****************************/
###
###/*----------------------------------------------------*/
###/* st x --> call safe_st_x (reg to store in r2)
### * st -x --> call safe_st_dec_x (reg to store in r2) */
###.global safe_st_x
###    .type safe_st_x, @function
###.global safe_st_dec_x
###    .type safe_st_dec_x, @function
###safe_st_dec_x:
###    sbiw r26, 1
###safe_st_x:
###/* CHECK X reg (r26:r27) */
###    st X, r2
###    ret
###
### /* st x+ --> call safe_st_inc_x (reg to store in r2) */
###.global safe_st_inc_x
###    .type safe_st_inc_x, @function
###safe_st_inc_x:
###/* CHECK X reg (r26:r27) */
###    st X, r2
###    adiw r26, 1
###    ret
###/*----------------------------------------------------*/
###/* st y --> call safe_st_y (reg to store in r2)
### * st -y --> call safe_st_dec_y (reg to store in r2) */
###.global safe_st_y
###    .type safe_st_y, @function
###.global safe_st_dec_y
###    .type safe_st_dec_y, @function
###safe_st_dec_y:
###    sbiw r28, 1
###safe_st_y:
###/* CHECK Y reg (r28:r29) */
###    st Y, r2
###    ret
###
### /* st y+ --> call safe_st_inc_y (reg to store in r2) */
###.global safe_st_inc_y
###    .type safe_st_inc_y, @function
###safe_st_inc_y:
###/* CHECK Y reg (r28:r29) */
###    st Y, r2
###    adiw r28, 1
###    ret
###/*----------------------------------------------------*/
###/* st z --> call safe_st_z (reg to store in r2)
### * st -z --> call safe_st_dec_z (reg to store in r2) */
###.global safe_st_z
###    .type safe_st_z, @function
###.global safe_st_dec_z
###    .type safe_st_dec_z, @function
###safe_st_dec_z:
###    sbiw r30, 1
###safe_st_z:
###/* CHECK Z reg (r30:r31) */
###    st Z, r2
###    ret
###
### /* st z+ --> call safe_st_inc_z (reg to store in r2) */
###.global safe_st_inc_z
###    .type safe_st_inc_z, @function
###safe_st_inc_z:
###/* CHECK Z reg (r30:r31) */
###    st Z, r2
###    adiw r30, 1
###    ret
###/*----------------------------------------------------*/
###/* std y+q --> call safe_std_y_q (reg to store in r2) */
###.altmacro
###.macro safe_std_y l=1
###.global safe_std_y_\l
###    .type safe_std_y_\l, @function
###safe_std_y_\l:
###    adiw r28,\l
###/* CHECK Y reg (r28:r29) */
###    st Y, r2
###    sbiw r28, \l
###    ret
###.if \l-63
###safe_std_y %l+1
###.endif
###.endm
###safe_std_y
###/*----------------------------------------------------*/
###/* std z+q --> call safe_std_z_q (reg to store in r2) */
###.altmacro
###.macro safe_std_z l=1
###.global safe_std_z_\l
###    .type safe_std_z_\l, @function
###safe_std_z_\l:
###    adiw r30,\l
###/* CHECK Z reg (r30:r31) */
###    st Z, r2
###    sbiw r30, \l
###    ret
###.if \l-63
###safe_std_z %l+1
###.endif
###.endm
###safe_std_z
###
###/* TODO: POP/PUSH */
###
###/* Checked statically by verifier: sts, out, sbi/cbi, bset/bclr ... */
