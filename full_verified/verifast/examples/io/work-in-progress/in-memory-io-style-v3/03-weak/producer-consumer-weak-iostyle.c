#include <malloc.h>
#include <stdlib.h> // abort()
//@ #include <ghost_cells.gh>

/**
 * Motivating example for in-memory i/o. This example has i/o style.
 *
 * Nice properties:
 * - knowledge about I/O happening in another thread can be used
 *   (here: the reader knows what the writer will write)
 *   (ESOP paper did not show a way to do that)
 * - this knowledge about relationship between threads is rather
 *   user-defined in a style chosen by a user, in contrast to
 *   only definable in one fixed style (e.g. IGR).
 *
 * Issues:
 * - No proper support for split/join (e.g. the writer can write "1,2,3,4" and "5,6,7,8,9,10"
        interleaved: it can e.g. write "1,5,6,2,3,8,9,10,4").
 * - A solution of the constraints given by the I/O style contract, must be given upfront.
 *   (here the solution is that "1,2,3,4,5,6,7,8,9,10" will be written and read, in practice
 *    the solutions will be more complicated)
 * - might be an issue: nondeterminism (not looked into yet)
 * - Closing the I/O style predicates is cumbersome.
 * - No compositionality: the push and pop functions know how they are used and cannot be reused
 *   for something else.
 */

#include "../../../../vstte2012/problem3/problem3.h" // Ring buffer
#include <threading.h>

struct buffer {
  struct ring_buffer *ring_buffer;
  struct mutex *mutex;
  struct mutex_cond *cond_can_push;
  struct mutex_cond *cond_can_pop;
};


/*@
predicate_ctor buffer_protected(struct buffer *buffer, int id_text, int id_progress_read, int id_progress_write)() =
  buffer->ring_buffer |-> ?ring_buffer
  &*& ring_buffer(ring_buffer, ?size, ?contents)
  
  &*& [_]ghost_cell<list<int> >(id_text, ?alltext)
  &*& [1/2]ghost_cell<int>(id_progress_read, ?n_read)
  &*& [1/2]ghost_cell<int>(id_progress_write, ?n_write)
  &*& take(n_write - n_read, drop(n_read, alltext)) == contents
;

predicate buffer(struct buffer *buffer, int id_text, int id_progress_read, int id_progress_write;) =
  buffer-> mutex|-> ?mutex
  &*& mutex(mutex, ((buffer_protected)(buffer, id_text, id_progress_read, id_progress_write)))
  &*& buffer->cond_can_push |-> ?cond_can_push
  &*& mutex_cond(cond_can_push, mutex)
  &*& buffer->cond_can_pop |-> ?cond_can_pop
  &*& mutex_cond(cond_can_pop, mutex)
  
  &*& malloc_block_buffer(buffer);


predicate token(int id_progress, int val) =
  [1/2]ghost_cell<int>(id_progress, val);

// i/o operation (here both reading and writing, depending on id_progress)
predicate op(int id_text, int id_progress, int n1, int c, int n2) =
  [_]ghost_cell<list<int> >(id_text, ?all_text)
  &*& c == nth(n1, all_text)
  &*& n2 == n1 + 1;

// a higher-level i/o operation build on top of the "op" i/o operation.
predicate ops(int id_text, int id_progress, int n1, list<int> cs, int n2) =
  cs == nil ?
    n2 == n1
  :
    op(id_text, id_progress, n1, head(cs), ?na)
    &*& ops(id_text, id_progress, na, tail(cs), n2)
;

@*/

struct buffer *create_buffer(int size)
//@ requires true &*& size > 0 &*& size * sizeof(int) < INT_MAX &*& [_]ghost_cell<list<int> >(?id_contents, ?alltext);
/*@ ensures
  [_]ghost_cell(id_contents, alltext)
  &*& result == 0 ?
    emp
  :
    buffer(result, id_contents, ?id_progress_read, ?id_progress_write)
    &*& token(id_progress_read, 0)
    &*& token(id_progress_write, 0);
@*/
{
  struct buffer *buffer = malloc(sizeof(struct buffer));
  if (buffer == 0){
    return 0;
  }
  struct ring_buffer *ring_buffer = ring_buffer_create(size);
  if (ring_buffer == 0){
    free(buffer);
    return 0;
  }
  buffer->ring_buffer = ring_buffer;
  //@ int id_progress_read = create_ghost_cell(0);
  //@ int id_progress_write = create_ghost_cell(0); 
  //@ close create_mutex_ghost_arg(buffer_protected(buffer, id_contents, id_progress_read, id_progress_write));
  //@ close exists<list<int> >(nil);
  //@ close buffer_protected(buffer, id_contents, id_progress_read, id_progress_write)();
  buffer->mutex = create_mutex();
  //@ close create_mutex_cond_ghost_args(buffer->mutex);
  buffer->cond_can_push = create_mutex_cond();
  //@ close create_mutex_cond_ghost_args(buffer->mutex);
  buffer->cond_can_pop = create_mutex_cond();

  return buffer;
  //@ close buffer(buffer, id_contents, id_progress_read, id_progress_write);
  //@ close token(id_progress_read, 0);
  //@ close token(id_progress_write, 0);
}
/*
void buffer_dispose(struct buffer *buffer)
//@ requires buffer(buffer, ?id1, ?id2, ?id3);
//@ ensures [1/2]ghost_cell<list<int> >(id_to_write, _) &*& [1/2]ghost_cell<list<int> >(id_to_read, _);
{
  //@ open buffer(_, _, _);
  mutex_cond_dispose(buffer->cond_can_push);
  mutex_cond_dispose(buffer->cond_can_pop);
  mutex_dispose(buffer->mutex);
  //@ open buffer_protected(buffer, id_to_read, id_to_write)();
  ring_buffer_dispose(buffer->ring_buffer);
  free(buffer);
}
*/

/** add to end of queue */
void push(struct buffer *buffer, int x)
/*@ requires
  [?f]buffer(buffer, ?id_text, ?id_progress_read, ?id_progress_write)
  &*& token(id_progress_write, ?t1)
  &*& op(id_text, id_progress_write, t1, x, ?t2);
@*/
/*@ ensures
  [f]buffer(buffer, id_text, id_progress_read, id_progress_write)
  &*& token(id_progress_write, t2);

@*/
{
  //@ open buffer(buffer, _, _, _);
  //@ assert [f]buffer->mutex |-> ?mutex;
  mutex_acquire(buffer->mutex);
  //@ open buffer_protected(buffer, id_text, id_progress_read, id_progress_write)();
  //@ open token(id_progress_write, ?n_write);
  //@ assert [_]ghost_cell<list<int> >(id_text, ?alltext);
  while (ring_buffer_is_full(buffer->ring_buffer))
  /*@ invariant
      buffer->ring_buffer |-> ?ring_buffer
      &*& [f]buffer->mutex |-> mutex
      &*& ring_buffer(ring_buffer, ?size, ?contents)
      &*& [f]buffer->cond_can_push |-> ?cond_can_push
      &*& [f]mutex_cond(cond_can_push, mutex)
      &*& mutex_held(mutex, (buffer_protected)(buffer, id_text, id_progress_read, id_progress_write), currentThread, f)
      
      &*& [_]ghost_cell<list<int> >(id_text, alltext)
      &*& [1/2]ghost_cell<int>(id_progress_read, ?n_read)
      &*& [1/2]ghost_cell<int>(id_progress_write, n_write)
      &*& [1/2]ghost_cell<int>(id_progress_write, n_write)
      &*& take(n_write - n_read, drop(n_read, alltext)) == contents
      ;
      
  @*/
  {
    //@ close buffer_protected(buffer, id_text, id_progress_read, id_progress_write)();
    mutex_cond_wait(buffer->cond_can_push, buffer->mutex);
    //@ open buffer_protected(buffer, id_text, id_progress_read, id_progress_write)();
  }
  
  bool was_empty = ring_buffer_is_empty(buffer->ring_buffer);

  ring_buffer_push(buffer->ring_buffer, x);

  if (was_empty){
    mutex_cond_signal(buffer->cond_can_pop);
  }
  
  
  //@ ghost_cell_mutate(id_progress_write, n_write + 1);
  //@ open op(_, _, _, _, _);
  //@ assert t2 == n_write + 1;
  
  //@ close token(id_progress_write, n_write + 1);
  
  //@ assert take(n_write - n_read, drop(n_read, alltext)) == contents;
  //@ assume (take(n_write+1 - n_read, drop(n_read, alltext)) == append(contents, cons(nth(n_write, alltext), nil)));
  
  //@ close buffer_protected(buffer, id_text, id_progress_read, id_progress_write)();
  mutex_release(buffer->mutex);
  //@ close [f]buffer(buffer, id_text, id_progress_read, id_progress_write);
}

/** read from beginning of queue (and remove that element) */
int pop(struct buffer *buffer)
/*@ requires
  [?f]buffer(buffer, ?id_text, ?id_progress_read, ?id_progress_write)
  &*& token(id_progress_read, ?t1)
  &*& op(id_text, id_progress_read, t1, ?c, ?t2);
@*/
/*@ ensures
  [f]buffer(buffer, id_text, id_progress_read, id_progress_write)
  &*& token(id_progress_read, t2)
  &*& result == c;
@*/
{
  //@ open buffer(buffer, _, _, _);
  //@ assert [f]buffer->mutex |-> ?mutex;
  mutex_acquire(buffer->mutex);
  //@ open buffer_protected(buffer, id_text, id_progress_read, id_progress_write)();
  //@ open token(id_progress_read, ?n_read);
  //@ assert [_]ghost_cell<list<int> >(id_text, ?alltext);
  while (ring_buffer_is_empty(buffer->ring_buffer))
  /*@ invariant
      
      buffer->ring_buffer |-> ?ring_buffer
      &*& [f]buffer->mutex |-> mutex
      &*& ring_buffer(ring_buffer, ?size, ?contents)
      &*& [f]buffer->cond_can_pop |-> ?cond_can_pop
      &*& [f]mutex_cond(cond_can_pop, mutex)
      &*& mutex_held(mutex, (buffer_protected)(buffer, id_text, id_progress_read, id_progress_write), currentThread, f)
      
      &*& [_]ghost_cell<list<int> >(id_text, alltext)
      &*& [1/2]ghost_cell<int>(id_progress_write, ?n_write)
      &*& [1/2]ghost_cell<int>(id_progress_read, n_read)
      &*& [1/2]ghost_cell<int>(id_progress_read, n_read)
      &*& take(n_write - n_read, drop(n_read, alltext)) == contents
      ;
  @*/
  {
    //@ close buffer_protected(buffer, id_text, id_progress_read, id_progress_write)();
    mutex_cond_wait(buffer->cond_can_pop, buffer->mutex);
    //@ open buffer_protected(buffer, id_text, id_progress_read, id_progress_write)();
  }
  
  bool was_full = ring_buffer_is_full(buffer->ring_buffer);
  
  int x = ring_buffer_pop(buffer->ring_buffer);
  
  if (was_full){
    mutex_cond_signal(buffer->cond_can_push);
  }
  
  //@ ghost_cell_mutate(id_progress_read, t2);
  //@ close token(id_progress_read, t2);
  //@ open op(_, _, _, c, _);
  
  //@ assert c == nth(t1, alltext);
  //@ assert x == head(contents);
  //@ assert x == head(take(n_write - n_read, drop(n_read, alltext)));
  //@ assume (x == nth(n_read, alltext));
  //@ assert c == x;
  
  //@ assert take(n_write - n_read, drop(n_read, alltext)) == contents;
  //@ assume (take(n_write - (n_read + 1), drop((n_read+1), alltext)) == tail(contents));
  
  //@ close buffer_protected(buffer, id_text, id_progress_read, id_progress_write)();
  mutex_release(buffer->mutex);
  //@ close [f]buffer(buffer, id_text, id_progress_read, id_progress_write);
  return x;
}

/*@
inductive thread_data = 
  thread_data(int content_id, int id_progress_read, int id_progress_write, int t1, int t2);
@*/

/*@ predicate_family_instance thread_run_pre(producer)(struct buffer *buffer, any p) =
  [1/2]buffer(buffer, ?id_contents, ?id_progress_read, ?id_progress_write)
  &*& ops(id_contents, id_progress_write, ?t1, {1,2,3,4,5,6,7,8,9,10}, ?t2)
  &*& token(id_progress_write, t1)
  &*& p == thread_data(id_contents, id_progress_read, id_progress_write, t1, t2);
@*/
/*@ predicate_family_instance thread_run_post(producer)(struct buffer *buffer, any p) =
  [1/2]buffer(buffer, ?id_contents, ?id_progress_read, ?id_progress_write)
  &*& token(id_progress_write, ?t2)
  &*& exists<int>(?t1)
  &*& p == thread_data(id_contents, id_progress_read, id_progress_write, t1, t2);
@*/

void producer(struct buffer *buffer) //@ : thread_run_joinable
//@ requires thread_run_pre(producer)(buffer, ?info) &*& lockset(currentThread, nil);
//@ ensures thread_run_post(producer)(buffer, info) &*& lockset(currentThread, nil);
{
  int x = 1;
  //@ open thread_run_pre(producer)(buffer, info);
  
  // let's save on proving list properties by unrolling the loop. 
  //@ open ops(?id_text, ?id_write, ?t1, _, ?t2);
  push(buffer, x);
  x++;
  
  //@ open ops(_, _, _, _, _);
  push(buffer, x);
  x++;
  //@ open ops(_, _, _, _, _);
  push(buffer, x);
  x++;
  //@ open ops(_, _, _, _, _);
  push(buffer, x);
  x++;
  //@ open ops(_, _, _, _, _);
  push(buffer, x);
  x++;
  //@ open ops(_, _, _, _, _);
  push(buffer, x);
  x++;
  //@ open ops(_, _, _, _, _);
  push(buffer, x);
  x++;
  //@ open ops(_, _, _, _, _);
  push(buffer, x);
  x++;
  //@ open ops(_, _, _, _, _);
  push(buffer, x);
  x++;
  //@ open ops(_, _, _, _, _);
  push(buffer, x);
  x++;
  //@ open ops(_, _, _, _, _);
  //@ close exists(t1);
  //@ close thread_run_post(producer)(buffer, info);
}

int consumer(struct buffer *buffer, int count)
/*@ requires
  [1/2]buffer(buffer, ?id_contents, ?id_progress_read, ?id_progress_write)
  &*& ops(id_contents, id_progress_read, ?t1, {1,2,3,4,5,6,7,8,9,10}, ?t2)
  &*& token(id_progress_read, t1)
;
@*/
/*@ ensures
  [1/2]buffer(buffer, id_contents, id_progress_read, id_progress_write)
  &*& token(id_progress_read, t2)
  &*& result == 55;
@*/
{
  int i;
  int sum = 0;
  
  //@ open ops(_, _, _, _, _);
  int pop_result = pop(buffer);
  sum = sum + pop_result;
  
  //@ open ops(_, _, _, _, _);
  pop_result = pop(buffer);
  sum = sum + pop_result;
  
  //@ open ops(_, _, _, _, _);
  pop_result = pop(buffer);
  sum = sum + pop_result;
  
  //@ open ops(_, _, _, _, _);
  pop_result = pop(buffer);
  sum = sum + pop_result;
  
  //@ open ops(_, _, _, _, _);
  pop_result = pop(buffer);
  sum = sum + pop_result;
  
  //@ open ops(_, _, _, _, _);
  pop_result = pop(buffer);
  sum = sum + pop_result;
  
  //@ open ops(_, _, _, _, _);
  pop_result = pop(buffer);
  sum = sum + pop_result;
  
  //@ open ops(_, _, _, _, _);
  pop_result = pop(buffer);
  sum = sum + pop_result;
  
  //@ open ops(_, _, _, _, _);
  pop_result = pop(buffer);
  sum = sum + pop_result;
  
  //@ open ops(_, _, _, _, _);
  pop_result = pop(buffer);
  sum = sum + pop_result;
  
  //@ open ops(_, _, _, _, _);
  return sum;
}

/*@
lemma void create_ops(int id_text, int id_op)
requires [_]ghost_cell<list<int> >(id_text, {1,2,3,4,5,6,7,8,9,10});
ensures ops(id_text, id_op, 0, {1,2,3,4,5,6,7,8,9,10}, 10);
{
  // let's save on writing loop invariants once more.
  int i = 0;
  
  close op(id_text, id_op, i, i+1, i+1);
  i=i+1;
  close op(id_text, id_op, i, i+1, i+1);
  i=i+1;
  close op(id_text, id_op, i, i+1, i+1);
  i=i+1;
  close op(id_text, id_op, i, i+1, i+1);
  i=i+1;
  close op(id_text, id_op, i, i+1, i+1);
  i=i+1;
  close op(id_text, id_op, i, i+1, i+1);
  i=i+1;
  close op(id_text, id_op, i, i+1, i+1);
  i=i+1;
  close op(id_text, id_op, i, i+1, i+1);
  i=i+1;
  close op(id_text, id_op, i, i+1, i+1);
  i=i+1;
  close op(id_text, id_op, i, i+1, i+1);
  i=i+1;
  assert i == 10;
  
  close ops(id_text, id_op, 10, {}, 10);
  close ops(id_text, id_op, 9, {10}, 10);  
  close ops(id_text, id_op, 8, {9,10}, 10);
  close ops(id_text, id_op, 7, {8,9,10}, 10);
  close ops(id_text, id_op, 6, {7,8,9,10}, 10);
  close ops(id_text, id_op, 5, {6,7,8,9,10}, 10);
  close ops(id_text, id_op, 4, {5,6,7,8,9,10}, 10);
  close ops(id_text, id_op, 3, {4,5,6,7,8,9,10}, 10);
  close ops(id_text, id_op, 2, {3,4,5,6,7,8,9,10}, 10);
  close ops(id_text, id_op, 1, {2,3,4,5,6,7,8,9,10}, 10);
  close ops(id_text, id_op, 0, {1,2,3,4,5,6,7,8,9,10}, 10);
}
@*/


int main()
//@ requires true;
//@ ensures result == 55;
{
  //@ int id_text = create_ghost_cell({1,2,3,4,5,6,7,8,9,10});
  //@ leak ghost_cell(id_text, _);
  struct buffer *buffer = create_buffer(2);
  if (buffer == 0){
    abort();
  }
  
  //@ assert buffer(_, _, ?id_read, ?id_write);
  //@ create_ops(id_text, id_write);
  //@ create_ops(id_text, id_read);
  
  //@ close thread_run_pre(producer)(buffer, thread_data(id_text, id_read, id_write, 0, 10));
  struct thread *thread = thread_start_joinable(producer, buffer);
  int result = consumer(buffer, 100);
  thread_join(thread);
  //@ open thread_run_post(producer)(buffer, _);
  return result;
  //@ leak exists(_);
  //@ leak token(_, _);
  //@ leak token(_, _);
  //@ leak [1/2]buffer(_, _, _, _);
  //@ leak [1/2]buffer(_, _, _, _);
}



struct cat_data{
  struct buffer *bufferA;
  struct buffer *bufferB;
};

/*@
inductive cat_data = cat_data(int, int, int, int);

predicate cat_io(int id_textA, int id_readA, int t_readA_1, int id_textB, int id_writeB, int t_writeB_1, list<int> contents, int t_readA_2, int t_writeB_2) =
  ops(id_textA, id_readA, t_readA_1, contents, t_readA_2)
  &*& ops(id_textB, id_writeB, t_writeB_1, contents, t_writeB_2)
;

@*/

/*@ predicate_family_instance thread_run_pre(cat)(struct cat_data *cat_data, any p) =
  cat_data->bufferA |-> ?bufferA 
  &*& cat_data->bufferB |-> ?bufferB
  &*& cat_io(?id_textA, ?id_readA, ?t_readA_1, ?id_textB, ?id_writeB, ?t_writeB_1, {1,2,3,4,5,6,7,8,9,10}, ?t_readA_2, ?t_writeB_2)
  &*& token(id_readA, t_readA_1)
  &*& token(id_writeB, t_writeB_1)
  &*& [1/2]buffer(bufferA, id_textA, id_readA, ?id_writeA)
  &*& [1/2]buffer(bufferB, id_textB, ?id_readB, id_writeB)
  &*& malloc_block_cat_data(cat_data)
  &*& p == cat_data(id_readA, t_readA_2, id_writeB, t_writeB_2)
;
@*/
/*@ predicate_family_instance thread_run_post(cat)(struct cat_data *cat_data, any p) =
  cat_data->bufferA |-> ?bufferA
  &*& cat_data->bufferB |-> ?bufferB
  &*& [1/2]buffer(bufferA, ?id_textA, ?id_readA, ?id_writeA)
  &*& [1/2]buffer(bufferB, ?id_textB, ?id_readB, ?id_writeB)
  &*& token(id_readA, ?t_readA_2)
  &*& token(id_writeB, ?t_writeB_2)
  &*& malloc_block_cat_data(cat_data)
  &*& p == cat_data(id_readA, t_readA_2, id_writeB, t_writeB_2)
;
@*/

void cat(struct cat_data *cat_data) //@ : thread_run_joinable
//@ requires thread_run_pre(cat)(cat_data, ?info);
//@ ensures thread_run_post(cat)(cat_data, info);
{
  //@ open thread_run_pre(cat)(cat_data, info);
 
  //@ open cat_io(?id_textA, _, _, ?id_textB, _, _, _, _, _);
  
  // Implementation chooses a buffer size of two integers.
  
  //@ open ops(id_textA, _, _, _, _);
  int pop_result1 = pop(cat_data->bufferA);
  //@ open ops(id_textA, _, _, _, _);
  int pop_result2 = pop(cat_data->bufferA);
  
  //@ open ops(id_textB, _, _, _, _);
  push(cat_data->bufferB, pop_result1);
  //@ open ops(id_textB, _, _, _, _);
  push(cat_data->bufferB, pop_result2);
  
  //@ open ops(id_textA, _, _, _, _);
  pop_result1 = pop(cat_data->bufferA);
  //@ open ops(id_textA, _, _, _, _);
  pop_result2 = pop(cat_data->bufferA);
  
  //@ open ops(id_textB, _, _, _, _);
  push(cat_data->bufferB, pop_result1);
  //@ open ops(id_textB, _, _, _, _);
  push(cat_data->bufferB, pop_result2);
  
  //@ open ops(id_textA, _, _, _, _);
  pop_result1 = pop(cat_data->bufferA);
  //@ open ops(id_textA, _, _, _, _);
  pop_result2 = pop(cat_data->bufferA);
  
  //@ open ops(id_textB, _, _, _, _);
  push(cat_data->bufferB, pop_result1);
  //@ open ops(id_textB, _, _, _, _);
  push(cat_data->bufferB, pop_result2);
  
  //@ open ops(id_textA, _, _, _, _);
  pop_result1 = pop(cat_data->bufferA);
  //@ open ops(id_textA, _, _, _, _);
  pop_result2 = pop(cat_data->bufferA);
  
  //@ open ops(id_textB, _, _, _, _);
  push(cat_data->bufferB, pop_result1);
  //@ open ops(id_textB, _, _, _, _);
  push(cat_data->bufferB, pop_result2);
  
  //@ open ops(id_textA, _, _, _, _);
  pop_result1 = pop(cat_data->bufferA);
  //@ open ops(id_textA, _, _, _, _);
  pop_result2 = pop(cat_data->bufferA);
  
  //@ open ops(id_textB, _, _, _, _);
  push(cat_data->bufferB, pop_result1);
  //@ open ops(id_textB, _, _, _, _);
  push(cat_data->bufferB, pop_result2);
  
  //@ open ops(id_textA, _, _, _, _);
  //@ open ops(id_textB, _, _, _, _);
  
  //@ close thread_run_post(cat)(cat_data, info);
}

/*
 * producer writes {1,2,3,4,5,6,7,8,9,10} to buffer A
 * cat reads from buffer A and writes to buffer B. Cat can do buffering.
 * Consumer reads from buffer B and sums up the numbers it reads.
 */
int main_with_cat()
//@ requires true;
//@ ensures result == 55;
{
  //@ int id_text = create_ghost_cell({1,2,3,4,5,6,7,8,9,10});
  //@ leak ghost_cell(id_text, _);
  struct buffer *bufferA = create_buffer(2);
  struct buffer *bufferB = create_buffer(2);
  if (bufferA == 0 || bufferB == 0){
    abort();
  }
  
  //@ assert buffer(bufferA, _, ?id_readA, ?id_writeA);
  //@ assert buffer(bufferB, _, ?id_readB, ?id_writeB);
  
  // producer:
  //@ create_ops(id_text, id_writeA);
  //@ close thread_run_pre(producer)(bufferA, thread_data(id_text, id_readA, id_writeA, 0, 10));
  
  // cat:
  //@ create_ops(id_text, id_readA);
  //@ create_ops(id_text, id_writeB);
  //@ close cat_io(id_text, id_readA, 0, id_text, id_writeB, 0, {1,2,3,4,5,6,7,8,9,10}, ?t_readA_2, ?t_writeB_2);
  struct cat_data *cat_data = malloc(sizeof(struct cat_data));
  if (cat_data == 0){
    abort();
  }
  cat_data -> bufferA = bufferA;
  cat_data -> bufferB = bufferB;
  //@ close thread_run_pre(cat)(cat_data, cat_data(id_readA, t_readA_2, id_writeB, t_writeB_2));
  
  // consumer:
  //@ create_ops(id_text, id_readB);
  
  // start executing
  struct thread *thread_producer = thread_start_joinable(producer, bufferA);
  struct thread *thread_cat = thread_start_joinable(cat, cat_data);
  int result = consumer(bufferB, 100);
  
  thread_join(thread_producer);
  thread_join(thread_cat);
  //@ open thread_run_post(producer)(bufferA, _);
  return result;
  //@ leak exists(_);
  //@ leak token(_, _);
  //@ leak token(_, _);
  //@ leak [1/2]buffer(_, _, _, _);
  //@ leak [1/2]buffer(_, _, _, _);
  //@ leak thread_run_post(cat)(_, _);
}
