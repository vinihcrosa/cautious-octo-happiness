#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int* createArray(int size);
void printArray(int* array, int size);
void fillArray(int* array, int size);
void fillArrayWithRandom(int* array, int size);
void freeArray(int* array);
int* copyArray(int* array, int size);

int* createArray(int size)
{
  int* array = (int*) malloc(size * sizeof(int));
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
  for (int i = 0; i < size; i++)
  {
    array[i] = rand() % 100;
  }
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
