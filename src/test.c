
// a basic main file to test things with

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#include "chase_lev.h"

ws_deque *q;

unsigned long *sums;
unsigned long *steal_nums;

// takes in a number of pushes to make
void pusher(void *n) {

  unsigned long pushes = (unsigned long) n;

  for(unsigned long i = 0; i < pushes; i++) {
    push_bottom(q, (void *)(i+1));
  }

  pthread_exit(NULL);

}

// takes in a thread id
void stealer(void *n) {

  unsigned long tid = (unsigned long) n;
  unsigned long value;  

  unsigned long steal_num = steal_nums[tid];

  for(unsigned long i = 0; i < steal_num; i++) {
    value = (unsigned long) steal(q);
    if(value == 0) {
      i--; // zero means we failed to steal something
    } else {
      sums[tid] = sums[tid] + value;
    }
  }
  
  pthread_exit(NULL);

}

int main(int argc, char *argv[]) {

  // a "real" test of the Chase-Lev deque

  int n = atoi(argv[1]);

  q = ws_queue_build();

  pthread_t pushert;
  pthread_t stealert[n];

  unsigned long rsums[n];
  unsigned long rsteal_nums[n];

  sums = rsums;
  steal_nums = rsteal_nums;

  unsigned long pushes = atoi(argv[2]);
  unsigned long steals = pushes / n;

  for(int i = 0; i < n; i++) {
    sums[i] = 0;
    steal_nums[i] = steals;
    printf("initial sum setting: %ld\n", steal_nums[i]);
  }

  for( unsigned long i = 0; i < n; i++) {
    pthread_create(&stealert[i], NULL, stealer, (void *) i);
  }

  pthread_create(&pushert, NULL, pusher, (void *) pushes);

  pthread_join(pushert, NULL);

  for( int i = 0; i<n; i++ ) {
    pthread_join(stealert[i], NULL);
  }

  unsigned long final_sum = 0;
  for( int i = 0; i < n; i++ ) {
    printf("prev sum: %ld, i: %d, to add: %ld\n", final_sum, i, sums[i]);
    final_sum = final_sum + sums[i];
  }

  unsigned long expected = (pushes * (pushes + 1)) / 2;

  printf("expected %ld got %ld\n", expected, final_sum);

  printf("queue size: %d\n", (q->bottom - q->top));

}

  /*
  for(int i = 1; i < 10001; i++) {
    push_bottom(q, (void*)i);
  }

  int sum = 0;
  for(int i = 1; i < 10001; i++) {
    if(i % 2) {
      sum = sum + (int)steal(q);
    } else {
      sum = sum + (int)pop_bottom(q);
    }
  }

  int should_be = (10000 * 10001) / 2;

  printf("got %d should be %d\n", sum, should_be);
  */

  /*
  circular_array *a = ca_build(2);

  int i;
  for( i = 0; i < 3; i++) {
    ca_put(a, i, (void*)(i*5));
  }

  for( i = 0; i < 3; i++) {
    printf("value: %d\n", (int)ca_get(a, i));
  }

  circular_array *a2 = ca_grow(a, 3, -1);
  
  for( i = 0; i < 3; i++) {
    printf("value: %d\n", (int)ca_get(a2, i));
  }

  ca_put(a2, 3, (void*)120);

  for( i = 0; i < 4; i++) {
    printf("value: %d\n", (int)ca_get(a2, i));
  }
  */
