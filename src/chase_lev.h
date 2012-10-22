#ifndef CHASE_LEV_H
#define CHASE_LEV_H

#include <stdbool.h>
#include <glib.h>

#define LOG_INITIAL_SIZE 2

typedef struct {
  unsigned long int log_size;
  void **segment;
} circular_array;

circular_array * ca_build(unsigned long int log_size);

void * ca_get(circular_array *a, long int i);

void ca_put(circular_array *a, long int i, void *o);

circular_array * ca_grow(circular_array *a, long int b, long int t);

long int ca_size(circular_array *a);

typedef struct {
  long int bottom;
  void * volatile* top;
  circular_array *active_array;
} ws_deque;

ws_deque * ws_queue_build();

void push_bottom(ws_deque *q, void *value);

void * pop_bottom(ws_deque *q);

void * steal(ws_deque *q);

#endif
