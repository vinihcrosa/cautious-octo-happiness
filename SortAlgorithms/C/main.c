#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "utils.h"
#include "sortAlgorithms.h"

typedef struct
{
  const char *name;
  void (*function)(int *, int);
} SortAlgorithmEntry;

static double diffSeconds(struct timespec end, struct timespec start);
static double benchmarkSort(FILE *baseFile, int tam, void (*sortFunction)(int *, int), int printResult);
static const SortAlgorithmEntry *findSortAlgorithm(const char *name);

static const SortAlgorithmEntry SORT_ALGORITHMS[] = {
    {"bubbleSort", bubbleSort},
    {"selectionSort", selectionSort},
    {"insertionSort", insertionSort},
    {"quickSort", quickSort},
    {"mergeSort", mergeSort},
    {"heapSort", heapSort},
};
static const size_t SORT_ALGORITHMS_COUNT = sizeof(SORT_ALGORITHMS) / sizeof(SORT_ALGORITHMS[0]);

int main(int argc, char *argv[])
{
  if (argc < 3)
  {
    printf("Uso: %s <tamanho_array> <algoritmo_1> [algoritmo_2 ... algoritmo_n]\n", argv[0]);
    return 1;
  }

  int tam = atoi(argv[1]);
  if (tam <= 0)
  {
    fprintf(stderr, "Tamanho invalido: %s\n", argv[1]);
    return 1;
  }

  int requestedAlgorithms = argc - 2;
  printf("=== Benchmark de algoritmos de ordenacao ===\n");
  printf("Tamanho do array base: %d elementos\n", tam);
  printf("Algoritmos solicitados (%d): ", requestedAlgorithms);
  for (int i = 2; i < argc; i++)
  {
    printf("%s%s", argv[i], (i < argc - 1) ? ", " : "\n");
  }

  FILE *baseFile = tmpfile();
  if (baseFile == NULL)
  {
    perror("tmpfile");
    return 1;
  }

  int *baseArray = createArray(tam);
  if (baseArray == NULL)
  {
    fprintf(stderr, "Nao foi possivel alocar o array base.\n");
    fclose(baseFile);
    return 1;
  }
  fillArrayWithRandom(baseArray, tam);
  if (tam < 100)
  {
    printf("Array base (antes da ordenacao):\n");
    printArray(baseArray, tam);
  }
  size_t written = fwrite(baseArray, sizeof(int), (size_t)tam, baseFile);
  if (written != (size_t)tam)
  {
    fprintf(stderr, "Falha ao escrever o array base no arquivo temporario.\n");
    freeArray(baseArray);
    fclose(baseFile);
    return 1;
  }
  if (fflush(baseFile) != 0)
  {
    perror("fflush");
    freeArray(baseArray);
    fclose(baseFile);
    return 1;
  }
  freeArray(baseArray);

  int executed = 0;
  for (int argIndex = 2; argIndex < argc; argIndex++)
  {
    const char *algorithmName = argv[argIndex];
    const SortAlgorithmEntry *entry = findSortAlgorithm(algorithmName);
    if (entry == NULL)
    {
      fprintf(stderr, "[Aviso] Algoritmo desconhecido: %s. Ignorando.\n", algorithmName);
      continue;
    }

    executed++;
    printf("\n[%d/%d] Executando %s...\n", executed, requestedAlgorithms, entry->name);
    double elapsed = benchmarkSort(baseFile, tam, entry->function, tam < 100);
    if (elapsed < 0.0)
    {
      fprintf(stderr, "Falha ao executar %s.\n", entry->name);
      fclose(baseFile);
      return 1;
    }
    printf("[%s] Tempo decorrido: %.6f s\n", entry->name, elapsed);
  }

  fclose(baseFile);

  if (executed == 0)
  {
    fprintf(stderr, "Nenhum algoritmo valido foi informado.\n");
    return 1;
  }

  printf("\nBenchmark finalizado: %d algoritmo(s) executado(s) com sucesso.\n", executed);
  return 0;
}

static double benchmarkSort(FILE *baseFile, int tam, void (*sortFunction)(int *, int), int printResult)
{
  int *arr = createArray(tam);
  if (arr == NULL)
  {
    fprintf(stderr, "Falha ao alocar memoria para o algoritmo corrente.\n");
    return -1.0;
  }
  if (fseek(baseFile, 0, SEEK_SET) != 0)
  {
    perror("fseek");
    freeArray(arr);
    return -1.0;
  }
  size_t read = fread(arr, sizeof(int), (size_t)tam, baseFile);
  if (read != (size_t)tam)
  {
    fprintf(stderr, "Falha ao ler o array base do arquivo temporario (lidos %zu de %d elementos).\n", read, tam);
    freeArray(arr);
    return -1.0;
  }

  struct timespec start, end;
  if (clock_gettime(CLOCK_MONOTONIC, &start) != 0)
  {
    perror("clock_gettime");
    freeArray(arr);
    return -1.0;
  }

  sortFunction(arr, tam);

  if (clock_gettime(CLOCK_MONOTONIC, &end) != 0)
  {
    perror("clock_gettime");
    freeArray(arr);
    return -1.0;
  }

  if (printResult)
  {
    printf("Resultado ordenado:\n");
    printArray(arr, tam);
  }

  freeArray(arr);
  return diffSeconds(end, start);
}

static const SortAlgorithmEntry *findSortAlgorithm(const char *name)
{
  for (size_t i = 0; i < SORT_ALGORITHMS_COUNT; i++)
  {
    if (strcmp(SORT_ALGORITHMS[i].name, name) == 0)
    {
      return &SORT_ALGORITHMS[i];
    }
  }
  return NULL;
}

static double diffSeconds(struct timespec end, struct timespec start)
{
  double seconds = (double)(end.tv_sec - start.tv_sec);
  long nanosecDelta = end.tv_nsec - start.tv_nsec;
  return seconds + (double)nanosecDelta / 1e9;
}
