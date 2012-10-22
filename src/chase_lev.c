
// work-stealing deque based on the Chase-Lev paper

#include <stdbool.h>
#include <malloc.h>
#include <glib.h>

#include "chase_lev.h"

circular_array * ca_build(unsigned long int log_size) {

  circular_array* ca = (circular_array*) malloc(sizeof(circular_array));

  ca->log_size = log_size;
  long int real_size = 1 << log_size;
  ca->segment = (void*) malloc(real_size * sizeof(void *));

  return ca;

}

void * ca_get(circular_array *a, long int i) {

  void *segment = a->segment;
  long int index = i % ca_size(a);
  void *res = ((void**)segment)[index];
  return res;

}

void ca_put(circular_array *a, long int i, void *o) {

  void *segment = a->segment;
  ((void**)segment)[i % ca_size(a)] = o;

}

circular_array * ca_grow(circular_array *a, long int b, long int t) {

  circular_array *na = ca_build(a->log_size + 1);

  for (long int i = t; i < b; i++) {
    ca_put(na, i, ca_get(a, i));
  }

  return na;

}

long ca_size(circular_array *a) {

  return (1 << a->log_size);

}

ws_deque * ws_queue_build() {

  ws_deque *q = (ws_deque*) malloc(sizeof(ws_deque));

  q->bottom = 0;
  g_atomic_pointer_set(&(q->top), (long int) 0);
  q->active_array = ca_build(LOG_INITIAL_SIZE);

  return q;

}

void push_bottom(ws_deque *q, void *value) {

  long int b = q->bottom;
  long int t = (long int) g_atomic_pointer_get(&(q->top));
  circular_array *a = q->active_array;
  long int a_size = ca_size(a);
  long int size = b - t;

  if(size >= (a_size - 1)) {
    a = ca_grow(a, b, t);
    q->active_array = a;
  }

  ca_put(a, b, value);
  q->bottom = b + 1;
  
}

void * pop_bottom(ws_deque *q) {

  long int b = q->bottom;
  circular_array *a = q->active_array;

  b = b - 1;
  q->bottom = b;

  long int t = (long int) g_atomic_pointer_get(&(q->top));
  long int size = b - t;
  long int zero = 0;

  if(size < zero) {
    q->bottom = t;
    return NULL;
  }

  void *value = ca_get(a, b);

  if(size > 0) {
    return value;
  }

  // this is the case for exactly one element, this is now the top so act accordingly

  long int new_t = t+1;
  gboolean cas = g_atomic_pointer_compare_and_exchange((void * volatile*)&(q->top), 
                                                       (void*)t, 
                                                       (void*)new_t);

  if( !cas ) {
    value = NULL;
  }
  
  q->bottom = t+1;

  return value;

}

void * steal(ws_deque *q) {

  long int t = (long int) g_atomic_pointer_get(&(q->top));
  long int b = q->bottom;
  circular_array *a = q->active_array;
  long int size = b - t;

  if( size <= 0 ) {
    return NULL;
  }

  void *value = ca_get(a, t);

  long int new_t = t + 1; // top stuff
  gboolean cas = g_atomic_pointer_compare_and_exchange((void * volatile*)&(q->top), 
                                                       (void*)t, 
                                                       (void*)new_t);

  if( !cas ) {
    return NULL;
  }

  return value;

}

