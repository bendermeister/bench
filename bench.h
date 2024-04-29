#ifndef BENCH_H_
#define BENCH_H_

#include <stdint.h>
#include <stdio.h>
#include <time.h>

typedef struct bench_timer_t bench_timer_t;
struct bench_timer_t {
  uint64_t sum;
  struct timespec end;
  struct timespec start;
};

#ifdef __GNUC__
#define BENCH_UNLIKELY(A) __builtin_expect((A), 0)
#else
#define BENCH_UNLIKELY(A)
#endif // __GNUC__

#define BENCH_NSECS_IN_SEC 1000000000

static void bench_timer_start(bench_timer_t *timer) {
  clock_gettime(CLOCK_MONOTONIC, &timer->start);
}

static uint64_t bench_timer_timespec_to_uint64_t(const struct timespec *ts) {
  return ts->tv_sec * BENCH_NSECS_IN_SEC + ts->tv_nsec;
}

static void bench_timespec_normalize(struct timespec *spec) {
  while (BENCH_UNLIKELY(spec->tv_nsec >= BENCH_NSECS_IN_SEC)) {
    spec->tv_sec += 1;
    spec->tv_nsec -= BENCH_NSECS_IN_SEC;
  }
}

static void bench_timer_end(bench_timer_t *timer) {
  clock_gettime(CLOCK_MONOTONIC, &timer->end);

  bench_timespec_normalize(&timer->start);
  bench_timespec_normalize(&timer->end);

  timer->sum = bench_timer_timespec_to_uint64_t(&timer->end) -
               bench_timer_timespec_to_uint64_t(&timer->start);
}

static void bench_timer_combine(bench_timer_t *dest, bench_timer_t *src) {
  dest->sum += src->sum;
}

#define BENCH(FUNC, RUNS, WARMUP)                                              \
  ({                                                                           \
    bench_timer_t sum = {};                                                    \
    bench_timer_t timer;                                                       \
    for (size_t i = 0; i < WARMUP; ++i) {                                      \
      timer = FUNC;                                                            \
    }                                                                          \
    for (size_t i = 0; i < RUNS; ++i) {                                        \
      timer = FUNC;                                                            \
      bench_timer_combine(&sum, &timer);                                       \
    }                                                                          \
    double avg_nsec = (double)sum.sum / RUNS;                                  \
    printf("%s: %lfns \n", #FUNC, avg_nsec);                                   \
  })

#endif // BENCH_H_
