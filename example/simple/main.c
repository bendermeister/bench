#include "../../bench.h"
#include <uc/types.h>

#include <stdlib.h>

// Lets benchmark rand()

static BenchTimer BM_rand(void) {
  // above the timer start we can have setup code
  // create timer to measure what we want to measure
  BenchTimer timer = {};

  // start the timer
  bench_timer_start(&timer);
  // volatile so this doesn't get optimized away
  volatile int i = rand();

  // end the timer after we are done
  bench_timer_end(&timer);

  // below the timer end we can have cleanup code

  (void)i;

  // then we just return the timer
  return timer;
}

int main(void) {
  srand(time(NULL));

  // run BM_rand() 1000 times and run a warmup 10 times
  BENCH(BM_rand(), 10000, 10);

  return 0;
}
