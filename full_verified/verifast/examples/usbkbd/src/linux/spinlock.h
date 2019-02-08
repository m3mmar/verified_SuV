#ifndef __LINUX_SPINLOCK_H
#define __LINUX_SPINLOCK_H

#include <linux/spinlock_types.h>

/*
 * Specs for Linux's spinlocks.
 * 
 * Specs are a inspired by Verifast's thread.h mutex API
 */


// XXX consistency spin_lock <-> spinlock.

/*#
 *
 * You can lock a spinlock if you have a fraction of this predicate.
 */
/*@
predicate spinlock(spinlock_t *spinlock, predicate() p ;);
// XXX mutex uses p as output-argument. Why didn't I do that?
@*/

/*#
 *
 * Sometimes, the code initialisation happens before the
 * permissions the spinlock protects are initialised.
 * This is correct as long as the spinlock is in no execution path
 * acquired before the initialition of the protected items happens.
 * To support this, you can initialise in two phases, first in code
 * (with spin_lock_init), then in ghost-code (with spin_lock_ghost_init).
 */
/*@
predicate spinlock_deferred_ghost_init(spinlock_t *spinlock, predicate() p);
@*/

/*@
predicate spin_lock_init_ghost_arg(predicate() p) = true;
@*/

/**
 *
 * Spinlocks _do_ require initalization, either dynamically or statically (compile-time).
 * This function (originally a macro) initializes dynamically.
 */
// XXX we might want to add support for initialization that happens before
// the p-predicate is available (which is valid as long as the lock is not
// acquired before p is available), i.e. splitting initialization in
// code-initialization and ghost-initialization which might be after
// code following after code-initialization.
// We don't need this for now, so it's currently not supported.
void spin_lock_init(spinlock_t *spinlock);
/*@ requires
	spin_lock_init_ghost_arg(?p)
	
	// This does not only prevent you to write to the spinlock,
	// it also tries to prevent from creating a spinlock-predicate twice
	// which tries to prevent you from locking the spinlock twice.
	// &*& chars((void*)spinlock, ?spinlock_chars) &*& length(spinlock_chars) == sizeof(spinlock_t)
	// XXX ok spinlock_t is not a struct yet because of struct-inside-struct problems,
	// so we can't do it with chars, so workaround with integer(spinlock, _) for now.
	&*& integer(spinlock, _) // if changed, also change spin_lock_dispose.
;
@*/
/*@	ensures spinlock_deferred_ghost_init(spinlock, p);
@*/

/*@
// See spin_lock_
lemma void spin_lock_ghost_init(spinlock_t *spinlock);
	requires spinlock_deferred_ghost_init(spinlock, ?p) &*& p();
	ensures spinlock(spinlock, p);
@*/

/*@
// We have "half-init", so it's useful to have the inverse as well
// such that the user of this API can switch back and forth.
lemma void spin_lock_dispose_half(spinlock_t *spinlock);
	requires spinlock(spinlock, ?p);
	ensures spinlock_deferred_ghost_init(spinlock, p) &*& p();
@*/
	
/*@
lemma void spin_lock_dispose(spinlock_t *spinlock);
	requires spinlock_deferred_ghost_init(spinlock, ?p);
	ensures integer(spinlock, _);
@*/

/*@
predicate spinlock_held(
	spinlock_t *spinlock,
	predicate () p,
	int threadId,
	real frac);
@*/


// XXX this is actually a macro, flags is actually not passed by value,
// so the macro can take the address of it and stuff. But we can't.
// So we can't (but should) enforce that the user does not overwrite flags while
// holding the spinlock :(
// It would be easyer if it was passed as a pointer and compiler-optimized-away,
// but that's not the way it is done.
void spin_lock_irqsave(spinlock_t *spinlock, unsigned long flags);
//@ requires [?frac]spinlock(spinlock, ?p);
//@ ensures p() &*& spinlock_held(spinlock, p, currentThread, frac);

void spin_unlock_irqrestore(spinlock_t *spinlock, unsigned long flags);
//@ requires spinlock_held(spinlock, ?p, currentThread, ?frac) &*& p();
//@ ensures [frac]spinlock(spinlock, p);


#endif

