
// work-stealing deque based on the Chase-Lev paper

#include <stdbool.h>
#include <malloc.h>
#include <glib.h>

#include "chase_lev.h"

#define G_ATOMIC_LOCK_FREE

circular_array * ca_build(int log_size) {

  circular_array* ca = (circular_array*) malloc(sizeof(circular_array));

  ca->log_size = log_size;
  int real_size = 1 << log_size;
  ca->segment = (void*) malloc(real_size * sizeof(void *));

  return ca;

}

void * ca_get(circular_array *a, int i) {

  void *segment = a->segment;
  int index = i % ca_size(a);
  void *res = ((void**)segment)[index];
  return res;

}

void ca_put(circular_array *a, int i, void *o) {

  void *segment = a->segment;
  ((void**)segment)[i % ca_size(a)] = o;

}

circular_array * ca_grow(circular_array *a, int b, int t) {

  circular_array *na = ca_build(a->log_size + 1);

  for (int i = t; i < b; i++) {
    ca_put(na, i, ca_get(a, i));
  }

  return na;

}

int ca_size(circular_array *a) {

  return (1 << a->log_size);

}

ws_deque * ws_queue_build() {

  ws_deque *q = (ws_deque*) malloc(sizeof(ws_deque));

  q->bottom = 0;
  q->top = 0;
  q->active_array = ca_build(LOG_INITIAL_SIZE);

  return q;

}

// is this needed since we have real CAS in glibc?
bool cas_top(gint *memloc, int new_val) {

  gint old_val = *memloc;
  gboolean res = g_atomic_int_compare_and_exchange(memloc, old_val, new_val);

  return res;

}

void push_bottom(ws_deque *q, void *value) {

  int b = q->bottom;
  int t = g_atomic_int_get(&(q->top));
  circular_array *a = q->active_array;
  int a_size = ca_size(a);
  int size = b - t;

  if(size >= (a_size - 1)) {
    a = ca_grow(a, b, t);
    q->active_array = a;
  }

  ca_put(a, b, value);
  q->bottom = b + 1;
  
}

void * pop_bottom(ws_deque *q) {

  int b = q->bottom;
  circular_array *a = q->active_array;

  b = b - 1;
  q->bottom = b;

  int t = g_atomic_int_get(&(q->top));
  int size = b - t;

  if(size < 0) {
    q->bottom = t;
    return NULL;
  }

  void *value = ca_get(a, b);

  if(size > 0) {
    return value;
  }

  //  gboolean cas = cas_top(&(q->top), t+1);
  //int cas = __sync_val_compare_and_swap(&(q->top), t, t+1);
  int new_t = t+1;
  gboolean cas = g_atomic_int_compare_and_exchange(&(q->top), t, new_t);

  if( !cas ) {
    value = NULL;
  }
  
  q->bottom = t+1;

  return value;

}

void * steal(ws_deque *q) {

  int t = g_atomic_int_get(&(q->top));
  //  printf("addresses: %d %p %p\n", t, &t, &(q->top));
  int b = q->bottom;
  circular_array *a = q->active_array;
  int size = b - t;

  if( size <= 0 ) {
    return NULL;
  }

  void *value = ca_get(a, t);

  //gboolean cas = cas_top(&(q->top), t+1);
  //int cas = __sync_val_compare_and_swap(&(q->top), t, t+1);  

  int new_t = t + 1;
  gboolean cas = g_atomic_int_compare_and_exchange(&(q->top), t, new_t);

  if( !cas ) {
    return NULL;
  }

  return value;

}

