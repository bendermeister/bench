#include "../../bench.h"

#include <stdlib.h>
#include <uc/types.h>

#include <emmintrin.h>
#include <immintrin.h>

static BenchTimer BM_linear_find(usize arr_length) {
  int *arr = malloc(arr_length * sizeof(*arr));
  for (usize i = 0; i < arr_length; ++i) {
    arr[i] = rand();
  }

  BenchTimer timer = {};
  volatile int target = arr[rand() % arr_length];
  volatile bool found = false;

  bench_timer_start(&timer);
  for (usize i = 0; i < arr_length && !found; ++i) {
    found = arr[i] == target;
  }
  bench_timer_end(&timer);

  free(arr);
  return timer;
}

static BenchTimer BM_linear_simd128_find(const usize arr_length) {
  int *arr = malloc(arr_length * sizeof(*arr));
  for (usize i = 0; i < arr_length; ++i) {
    arr[i] = rand();
  }

  BenchTimer timer = {};
  volatile int target = arr[rand() % arr_length];
  volatile bool found = false;

  bench_timer_start(&timer);

  __m128i target_mask = _mm_set1_epi32(target);
  usize index = 0;
  for (; !found && arr_length - index >= 4; index += 4) {
    __m128i data = _mm_load_si128((void *)(arr + index));
    int bitmask = _mm_movemask_epi8(_mm_cmpeq_epi32(data, target_mask));
    if (bitmask) {
      found = true;
      index +=
          (__builtin_ctz(bitmask) >> 4); // TODO: is this correct am I retarded?
      break;
    }
  }
  for (; index < arr_length && !found; ++index) {
    found = arr[index] == target;
  }

  bench_timer_end(&timer);

  free(arr);
  return timer;
}

static BenchTimer BM_linear_simd256_find(const usize arr_length) {
  int *arr = aligned_alloc(32, arr_length * sizeof(*arr));
  for (usize i = 0; i < arr_length; ++i) {
    arr[i] = rand();
  }

  BenchTimer timer = {};
  volatile int target = arr[rand() % arr_length];
  volatile bool found = false;

  bench_timer_start(&timer);
  usize index = 0;

  // 256 bit linear find
  {
    __m256i target_mask = _mm256_set1_epi32(target);
    for (; !found && arr_length - index >= 8; index += 8) {
      __m256i data = _mm256_load_si256((void *)(arr + index));
      int bitmask = _mm256_movemask_epi8(_mm256_cmpeq_epi32(data, target_mask));
      if (bitmask) {
        found = true;
        index += (__builtin_ctz(bitmask) >>
                  4); // TODO: is this correct am I retarded?
        break;
      }
    }
  }
  // 128 bit linear find
  {
    __m128i target_mask = _mm_set1_epi32(target);
    if (!found && arr_length - index >= 4) {
      __m128i data = _mm_load_si128((void *)(arr + index));
      int bitmask = _mm_movemask_epi8(_mm_cmpeq_epi32(data, target_mask));
      if (bitmask) {
        found = true;
        index += (__builtin_ctz(bitmask) >>
                  4); // TODO: is this correct am I retarded?
      }
    }
    index += 4;
  }
  // 32 bit linear find
  {
    for (; index < arr_length && !found; ++index) {
      found = arr[index] == target;
    }
  }

  bench_timer_end(&timer);

  free(arr);
  return timer;
}

int main(void) {
  srand(time(NULL));

  BENCH(BM_linear_find(100), 10000, 100);
  BENCH(BM_linear_find(1000), 10000, 100);
  BENCH(BM_linear_find(10000), 10000, 100);
  BENCH(BM_linear_find(100000), 10000, 100);
  printf("\n");
  BENCH(BM_linear_simd128_find(100), 10000, 100);
  BENCH(BM_linear_simd128_find(1000), 10000, 100);
  BENCH(BM_linear_simd128_find(10000), 10000, 100);
  BENCH(BM_linear_simd128_find(100000), 10000, 100);
  printf("\n");
  BENCH(BM_linear_simd256_find(100), 10000, 100);
  BENCH(BM_linear_simd256_find(1000), 10000, 100);
  BENCH(BM_linear_simd256_find(10000), 10000, 100);
  BENCH(BM_linear_simd256_find(100000), 10000, 100);

  return 0;
}
