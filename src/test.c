
// a basic main file to test things with

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#include "chase_lev.h"

ws_deque *q;

unsigned long int *sums;
unsigned long int *steal_nums;

// takes in a number of pushes to make
void pusher(void *n) {

  unsigned long int pushes = (unsigned long int) n;

  for(unsigned long int i = 0; i < pushes; i++) {
    push_bottom(q, (void *)(i+1));
  }

  pthread_exit(NULL);

}

// takes in a thread id
void stealer(void *n) {

  unsigned long int tid = (unsigned long int) n;
  unsigned long int value;  

  unsigned long int steal_num = steal_nums[tid];

  for(unsigned long int i = 0; i < steal_num; i++) {
    value = (unsigned long int) steal(q);
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

  unsigned long int rsums[n];
  unsigned long int rsteal_nums[n];

  sums = rsums;
  steal_nums = rsteal_nums;

  unsigned long int pushes = atoi(argv[2]);
  unsigned long int steals = pushes / n;

  for(int i = 0; i < n; i++) {
    sums[i] = 0;
    steal_nums[i] = steals;
  }

  for( unsigned long int i = 0; i < n; i++) {
    pthread_create(&stealert[i], NULL, (void * (*)(void *)) stealer, (void *) i);
  }

  pthread_create(&pushert, NULL, (void * (*)(void *)) pusher, (void *) pushes);

  pthread_join(pushert, NULL);

  for( int i = 0; i<n; i++ ) {
    pthread_join(stealert[i], NULL);
  }

  unsigned long int final_sum = 0;
  for( int i = 0; i < n; i++ ) {
    printf("prev sum: %ld, i: %d, to add: %ld\n", final_sum, i, sums[i]);
    final_sum = final_sum + sums[i];
  }

  unsigned long int expected = (pushes * (pushes + 1)) / 2;

  printf("expected %ld got %ld\n", expected, final_sum);

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
