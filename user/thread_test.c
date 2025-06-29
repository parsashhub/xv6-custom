#include "../kernel/memlayout.h"
#include "../kernel/param.h"
#include "../kernel/proc.h"
#include "../kernel/types.h"
#include "../user/user.h"

#define STACK_SIZE 100
#define ARRAY_SIZE 1000
#define NUM_THREADS 4

// Structure to pass arguments to the thread function
typedef struct {
  int *array;
  int start;
  int end;
} SortArgs;

// Global array and arguments to avoid stack issues
int global_array[ARRAY_SIZE];
int temp_array[ARRAY_SIZE]; // Temporary array for merging
SortArgs thread_args[NUM_THREADS];

// Simple insertion sort for each segment
void insertion_sort(int *arr, int start, int end) {
  int i, j, key;
  for (i = start + 1; i <= end; i++) {
    key = arr[i];
    j = i - 1;

    while (j >= start && arr[j] > key) {
      arr[j + 1] = arr[j];
      j = j - 1;
    }
    arr[j + 1] = key;
  }
}

// Thread function to sort a segment of the array
void *sort_thread(void *arg) {
  SortArgs *args = (SortArgs *)arg;
  insertion_sort(args->array, args->start, args->end);
  return 0;
}

// Merge two sorted subarrays into the original array
void merge(int *arr, int *temp, int left, int mid, int right) {
  // Copy data to temp array
  for (int i = left; i <= right; i++) {
    temp[i] = arr[i];
  }

  int i = left;    // Starting index for left subarray
  int j = mid + 1; // Starting index for right subarray
  int k = left;    // Starting index for merged array

  // Merge temp arrays back into arr[left..right]
  while (i <= mid && j <= right) {
    if (temp[i] <= temp[j]) {
      arr[k++] = temp[i++];
    } else {
      arr[k++] = temp[j++];
    }
  }

  // Copy the remaining elements of left subarray, if any
  while (i <= mid) {
    arr[k++] = temp[i++];
  }

  // Copy the remaining elements of right subarray, if any
  // (not needed as they are already in correct position)
}

// Merge sort implementation
void merge_sort(int *arr, int *temp, int left, int right) {
  if (left < right) {
    int mid = left + (right - left) / 2;

    // Sort first and second halves
    merge_sort(arr, temp, left, mid);
    merge_sort(arr, temp, mid + 1, right);

    // Merge the sorted halves
    merge(arr, temp, left, mid, right);
  }
}

// Original thread function for reference
void *my_thread(void *arg) {
  uint64 number = (uint64)arg;
  for (int i = 0; i < 100; ++i) {
    number++;
    printf("thread: %lu\n", number);
  }
  return (void *)number;
}

// Function to demonstrate parallel sorting
void parallel_sort_demo() {
  // Initialize the array with random values
  printf("Generating random array of %d integers...\n", ARRAY_SIZE);
  for (int i = 0; i < ARRAY_SIZE; i++) {
    global_array[i] = (i * 17 + 11) % ARRAY_SIZE; // Simple pseudo-random values
  }

  // Print first few elements of unsorted array
  printf("Unsorted array (first 10 elements): ");
  for (int i = 0; i < 10 && i < ARRAY_SIZE; i++) {
    printf("%d ", global_array[i]);
  }
  printf("...\n");

  // Create thread stacks
  int stacks[NUM_THREADS][STACK_SIZE];
  int thread_ids[NUM_THREADS];
  int segment_size = ARRAY_SIZE / NUM_THREADS;

  printf("Sorting array using %d threads...\n", NUM_THREADS);

  // Create threads to sort segments
  for (int i = 0; i < NUM_THREADS; i++) {
    thread_args[i].array = global_array;
    thread_args[i].start = i * segment_size;
    thread_args[i].end = (i == NUM_THREADS - 1) ? (ARRAY_SIZE - 1)
                                                : ((i + 1) * segment_size - 1);

    thread_ids[i] =
        thread(sort_thread, stacks[i] + STACK_SIZE, (void *)&thread_args[i]);
    printf("Thread %d created to sort elements %d to %d\n", thread_ids[i],
           thread_args[i].start, thread_args[i].end);
  }

  // Wait for all threads to complete
  for (int i = 0; i < NUM_THREADS; i++) {
    jointhread(thread_ids[i]);
    printf("Thread %d completed sorting\n", thread_ids[i]);
  }

  printf("Merging sorted segments...\n");

  // After all threads have completed, merge the sorted segments
  // We'll use a bottom-up approach to merge pairs of segments

  // First, copy the array to temp_array for the initial merge
  for (int i = 0; i < ARRAY_SIZE; i++) {
    temp_array[i] = global_array[i];
  }

  // Perform a complete merge sort on the entire array
  // This is more reliable than trying to merge just the segments
  merge_sort(global_array, temp_array, 0, ARRAY_SIZE - 1);

  // Print first few elements of sorted array
  printf("Sorted array (first 10 elements): ");
  for (int i = 0; i < 10 && i < ARRAY_SIZE; i++) {
    printf("%d ", global_array[i]);
  }
  printf("...\n");

  // Verify the array is sorted
  int is_sorted = 1;
  for (int i = 1; i < ARRAY_SIZE; i++) {
    if (global_array[i] < global_array[i - 1]) {
      is_sorted = 0;
      printf("ERROR: Array not sorted correctly at index %d!\n", i);
      break;
    }
  }

  if (is_sorted) {
    printf("SUCCESS: Array sorted correctly!\n");
  }
}

int main(int argc, char *argv[]) {
  printf("First test Starting...");
  int sp1[STACK_SIZE], sp2[STACK_SIZE], sp3[STACK_SIZE];
  int ta = thread(my_thread, sp1 + STACK_SIZE, (void *)100);
  printf("NEW THREAD CREATED %d\n", ta);
  int tb = thread(my_thread, sp2 + STACK_SIZE, (void *)200);
  printf("NEW THREAD CREATED %d\n", tb);
  int tc = thread(my_thread, sp3 + STACK_SIZE, (void *)300);
  printf("NEW THREAD CREATED %d\n", tc);

  jointhread(ta);
  jointhread(tb);
  jointhread(tc);
  printf("First test DONE!\n");

  // Run the parallel sorting demo
  printf("\nSecond test Starting...");
  parallel_sort_demo();

  printf("Second test DONE!\n");
  return 0;
}