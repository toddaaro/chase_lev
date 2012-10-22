
// a basic main file to test things with

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#include "chase_lev.h"

ws_deque *q;

void pusher(void *n) {

  for(int i = 0; i < n; i++) {
    push_bottom(q, (void *)(i+1));
  }

  pthread_exit(NULL);

}

void stealer(void *n) {

  long sum = 0;
  int value;  

  for(int i = 0; i <n; i++) {
    value = (int)steal(q);
    if(value == 0) {
      i--;
    } else {
      sum = sum + value;
    }
  }
  
  pthread_exit(sum);

}

int main(int argc, char *argv[]) {

  // a real test of the Chase-Lev deque

  int n = atoi(argv[1]);

  q = ws_queue_build();

  pthread_t pushert;

  pthread_t stealert[n];
  int sums[n];

  long arg = 16384*120;
  long per_s = arg/n;

  for( int i = 0; i < n; i++) {
    pthread_create(&stealert[i], NULL, stealer, (void *)per_s);
  }

  pthread_create(&pushert, NULL, pusher, (void *)arg);

  pthread_join(pushert, NULL);

  for( int i = 0; i<n; i++ ) {
    pthread_join(stealert[i], &sums[i]);
  }

  int final_sum = 0;
  for( int i = 0; i < n; i++ ) {
    //printf("prev sum: %d, i: %d, to add: %d\n", final_sum, i, sums[i]);
    final_sum = final_sum + sums[i];
  }

  printf("expected %d got %d\n", ((arg * (arg +1))/2), final_sum);

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
