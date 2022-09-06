#include "refcnt.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>


#define NUM_ITERATIONS 10000

void* test_thread(void* arg)
{
  char* str = arg;
  for (int i = 0; i < NUM_ITERATIONS; i++) {
    char* str2 = refcnt_ref(str);
    fprintf(stderr, "Thread %u, %i: %s\n", (unsigned int)pthread_self(), i, str2);
    refcnt_unref(str2);
  }
  refcnt_unref(str);
  return NULL;
}

#define NUM_THREADS 128

int main(int argc, char **argv)
{
  // Create threads
  pthread_t threads[NUM_THREADS];
  // Create a new string that is count referenced
  char* str = refcnt_strdup("Hello, world!");
  // Start the threads, passing a new counted copy of the referece
  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_create(&threads[i], NULL, test_thread, refcnt_ref(str));
  }
  // We no longer own the reference
  refcnt_unref(str);
  // Whichever thread finishes last will free the string
  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }
  void* ptr = malloc(100);
  // This should cause a heap overflow while checking the magic num which the
  // sanitizer checks.
  // Leaving commented out for now
  // refcnt_ref(ptr);

  free(ptr);
  return 0;
}
