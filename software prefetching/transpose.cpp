#include <bits/types/clock_t.h>
#include <cstddef>
#include <emmintrin.h>
#include <iostream>
#include <malloc.h>
#include <string.h>
#include <time.h>

#define ENABLE_PREFETCH
typedef __m128d xmm_reg;

using namespace std;

inline void swap_block(xmm_reg *A, xmm_reg *B, size_t L) {
  xmm_reg *stop = A + L;

  do {
    xmm_reg tmpA = *A;
    xmm_reg tmpB = *B;
    *A++ = tmpB;
    *B++ = tmpA;
  } while (A < stop);
}
// Calculates the transpose of given matrix of blocks
void transpose_even(xmm_reg *T, size_t block, size_t x) {

  size_t row_size = block * x;
  size_t iter_size = row_size + block;

  //  End of entire matrix.
  xmm_reg *stop_T = T + row_size * x;
  xmm_reg *end = stop_T - row_size;

  //  Iterate each row.
  xmm_reg *y_iter = T;
  do {
    //  Iterate each column.
    xmm_reg *ptr_x = y_iter + block;
    xmm_reg *ptr_y = y_iter + row_size;

    do {

#ifdef ENABLE_PREFETCH
      _mm_prefetch((char *)(ptr_y + row_size), 3);
#endif

      swap_block(ptr_x, ptr_y, block);

      ptr_x += block;
      ptr_y += row_size;
    } while (ptr_y < stop_T);

    y_iter += iter_size;
  } while (y_iter < end);
}
int main() {

  size_t dimension = 4096;
  size_t block = 16;

  size_t words = block * dimension * dimension;
  size_t bytes = words * sizeof(xmm_reg);
  cout << "Total Bytes = " << bytes << endl;

  xmm_reg *T = (xmm_reg *)_mm_malloc(bytes, 16);

  if (T == NULL) {
    cout << "Memory Allocation Failure" << endl;
    exit(1);
  }
  memset(T, 0, bytes);

  cout << "Starting Data Transpose...   ";
  clock_t start = clock();
  transpose_even(T, block, dimension);
  clock_t end = clock();
  double total_time = (double)(end - start) / CLOCKS_PER_SEC;
  cout << "Done" << endl;
  cout << "Time: " << total_time << " seconds" << endl;
  _mm_free(T);

}