#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "utils.h"
#include "sortAlgorithms.h"

long testSortFunction(int tam, void (*sortFunction)(int *, int))
{
  clock_t start, end;
  int *arr = createArray(tam);
  if (arr == NULL)
  {
    fprintf(stderr, "Nao foi possivel criar o vetor de tamanho %d.\n", tam);
    return -1;
  }
  fillArrayWithRandom(arr, tam);
  start = clock();
  sortFunction(arr, tam);
  end = clock();
  if (tam < 100)
  {
    printArray(arr, tam);
  }
  freeArray(arr);
  return end - start;
}

int main(int argc, char *argv[])
{
  for (int i = 0; i < argc; i++)
  {
    printf("argv[%d] = %s\n", i, argv[i]);
  }

  if (argc < 3)
  {
    printf("Usage: %s <arr_len> <sort_algorithm>\n", argv[0]);
    return 1;
  }

  int tam = atoi(argv[1]);
  long time;

  char *sortAlgorithm = argv[2];

  if (strcmp(sortAlgorithm, "bubbleSort") == 0)
  {
    printf("Testing bubbleSort\n");
    time = testSortFunction(tam, bubbleSort);
  }
  else if (strcmp(sortAlgorithm, "selectionSort") == 0)
  {
    printf("Testing selectionSort\n");
    time = testSortFunction(tam, selectionSort);
  }
  else if (strcmp(sortAlgorithm, "insertionSort") == 0)
  {
    printf("Testing insertionSort\n");
    time = testSortFunction(tam, insertionSort);
  }
  else if (strcmp(sortAlgorithm, "quickSort") == 0)
  {
    printf("Testing quickSort\n");
    time = testSortFunction(tam, quickSort);
  }
  else if (strcmp(sortAlgorithm, "mergeSort") == 0)
  {
    printf("Testing mergeSort\n");
    time = testSortFunction(tam, mergeSort);
  }
  else if (strcmp(sortAlgorithm, "heapSort") == 0)
  {
    printf("Testing heapSort\n");
    time = testSortFunction(tam, heapSort);
  }
  else
  {
    printf("Invalid sort algorithm\n");
    return 1;
  }
  if (time < 0)
  {
    fprintf(stderr, "Execucao interrompida devido a erro anterior.\n");
    return 1;
  }

  printf("Time: %ld secs \n", time / CLOCKS_PER_SEC);

  return 0;
}
