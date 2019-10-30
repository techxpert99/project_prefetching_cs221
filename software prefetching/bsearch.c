#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define PREFETCH_ENABLED 1
int binarySearch(int *array, int number_of_elements, int key) {
  int low = 0, high = number_of_elements - 1, mid;
  while (low <= high) {
    mid = (low + high) / 2;
#ifdef PREFETCH_ENABLED
    __builtin_prefetch(&array[(mid + 1 + high) / 2], 0, 1);
    __builtin_prefetch(&array[(low + mid - 1) / 2], 0, 1);
#endif

    if (array[mid] < key)
      low = mid + 1;
    else if (array[mid] == key)
      return mid;
    else if (array[mid] > key)
      high = mid - 1;
  }
  return -1;
}
int main() {
  int SIZE = 1024 * 1024 * 512;
  int *array = malloc(SIZE * sizeof(int));
  for (int i = 0; i < SIZE; i++) {
    array[i] = i;
  }
  int NUM_LOOKUPS = 1024 * 1024 * 8;
  srand(time(NULL));
  int *lookups = malloc(NUM_LOOKUPS * sizeof(int));
  clock_t start = clock();
  for (int i = 0; i < NUM_LOOKUPS; i++) {
    lookups[i] = rand() % SIZE;
  }
  for (int i = 0; i < NUM_LOOKUPS; i++) {
    int result = binarySearch(array, SIZE, lookups[i]);
  }
  clock_t end = clock();
  double total_time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Done");
  printf("Time: %lF",total_time );
  free(array);
  free(lookups);
}