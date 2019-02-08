/* Replacement for default __do_copy_data injected by the linker. This version
 * uses LPM in stead of ELPM, because we disallow ELPM for now in application
 * code. As a result, applications can only be <= 64K, which is not a problem
 * because in this prototype we use the upper 64K for temporary storage during
 * OTA deployment. */

	.section .init4,"ax",@progbits
    .global __do_copy_data
__do_copy_data:
	ldi	r17, hi8(__data_end)
	ldi	r26, lo8(__data_start)
	ldi	r27, hi8(__data_start)
	ldi	r30, lo8(__data_load_start)
	ldi	r31, hi8(__data_load_start)
	rjmp	.L__do_copy_data_start
.L__do_copy_data_loop:
	lpm	r0, Z+
	st	X+, r0
.L__do_copy_data_start:
	cpi	r26, lo8(__data_end)
	cpc	r27, r17
	brne	.L__do_copy_data_loop
