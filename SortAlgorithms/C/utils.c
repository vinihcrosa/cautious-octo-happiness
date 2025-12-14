#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#ifdef _OPENMP
#include <omp.h>
#endif

static inline unsigned int fastRand(unsigned int* state);

int* createArray(int size);
void printArray(int* array, int size);
void fillArray(int* array, int size);
void fillArrayWithRandom(int* array, int size);
void freeArray(int* array);
int* copyArray(int* array, int size);

int* createArray(int size)
{
  if (size <= 0)
  {
    return NULL;
  }
  int* array = (int*) malloc((size_t) size * sizeof(int));
  if (array == NULL)
  {
    fprintf(stderr, "Falha ao alocar memoria para %d inteiros.\n", size);
  }
  return array;
}

void printArray(int* array, int size)
{
  for (int i = 0; i < size; i++)
  {
    printf("%d ", array[i]);
  }
  printf("\n");
}

void fillArray(int* array, int size)
{
  for (int i = 0; i < size; i++)
  {
    array[i] = i;
  }
}

void fillArrayWithRandom(int* array, int size)
{
  if (array == NULL || size <= 0)
  {
    return;
  }
#ifdef _OPENMP
#pragma omp parallel
  {
    unsigned int seed = (unsigned int) time(NULL) ^ (unsigned int) (omp_get_thread_num() + 1U) * 2654435761U;
#pragma omp for schedule(static)
    for (int i = 0; i < size; i++)
    {
      array[i] = (int) (fastRand(&seed) & 0x7fffffff);
    }
  }
#else
  unsigned int seed = (unsigned int) time(NULL);
  for (int i = 0; i < size; i++)
  {
    array[i] = (int) (fastRand(&seed) & 0x7fffffff);
  }
#endif
}

void freeArray(int* array)
{
  free(array);
}

int* copyArray(int* array, int size)
{
  int* copy = createArray(size);
  for (int i = 0; i < size; i++)
  {
    copy[i] = array[i];
  }
  return copy;
}

static inline unsigned int fastRand(unsigned int* state)
{
  *state = (*state * 1664525u) + 1013904223u;
  return *state;
}
