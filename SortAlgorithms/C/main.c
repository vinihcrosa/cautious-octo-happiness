#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <limits.h>

#include "utils.h"
#include "sortAlgorithms.h"

typedef struct
{
  const char *name;
  void (*function)(int *, int);
} SortAlgorithmEntry;

static double diffSeconds(struct timespec end, struct timespec start);
static double benchmarkSort(FILE *baseFile, int tam, long long offsetBytes, void (*sortFunction)(int *, int), int printResult);
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
  if (argc < 4)
  {
    printf("Uso: %s <tamanho_array> <qtd_amostras> <algoritmo_1> [algoritmo_2 ... algoritmo_n]\n", argv[0]);
    return 1;
  }

  int tam = atoi(argv[1]);
  if (tam <= 0)
  {
    fprintf(stderr, "Tamanho invalido: %s\n", argv[1]);
    return 1;
  }

  int samples = atoi(argv[2]);
  if (samples <= 0)
  {
    fprintf(stderr, "Quantidade de amostras invalida: %s\n", argv[2]);
    return 1;
  }

  int requestedAlgorithms = argc - 3;
  const SortAlgorithmEntry **selected = (const SortAlgorithmEntry **)malloc((size_t)requestedAlgorithms * sizeof(*selected));
  if (selected == NULL)
  {
    fprintf(stderr, "Falha ao alocar memoria para a lista de algoritmos.\n");
    return 1;
  }

  int selectedCount = 0;
  for (int argIndex = 3; argIndex < argc; argIndex++)
  {
    const char *algorithmName = argv[argIndex];
    const SortAlgorithmEntry *entry = findSortAlgorithm(algorithmName);
    if (entry == NULL)
    {
      fprintf(stderr, "[Aviso] Algoritmo desconhecido: %s. Ignorando.\n", algorithmName);
      continue;
    }
    selected[selectedCount++] = entry;
  }

  if (selectedCount == 0)
  {
    fprintf(stderr, "Nenhum algoritmo valido foi informado.\n");
    free(selected);
    return 1;
  }

  printf("=== Benchmark de algoritmos de ordenacao (media em %d amostra(s)) ===\n", samples);
  printf("Tamanho de cada array: %d elementos\n", tam);
  printf("Algoritmos selecionados (%d): ", selectedCount);
  for (int i = 0; i < selectedCount; i++)
  {
    printf("%s%s", selected[i]->name, (i < selectedCount - 1) ? ", " : "\n");
  }

  double *totalTimes = (double *)calloc((size_t)selectedCount, sizeof(double));
  if (totalTimes == NULL)
  {
    fprintf(stderr, "Falha ao alocar memoria para acumular resultados.\n");
    free(selected);
    return 1;
  }

  FILE *baseFile = tmpfile();
  if (baseFile == NULL)
  {
    perror("tmpfile");
    free(selected);
    free(totalTimes);
    return 1;
  }

  int *baseArray = createArray(tam);
  if (baseArray == NULL)
  {
    fprintf(stderr, "Nao foi possivel alocar buffer para gerar amostras.\n");
    fclose(baseFile);
    free(selected);
    free(totalTimes);
    return 1;
  }

  printf("\nGerando %d amostra(s)...\n", samples);
  for (int sample = 0; sample < samples; sample++)
  {
    fillArrayWithRandom(baseArray, tam);
    if (tam < 100 && sample == 0)
    {
      printf("Amostra %d (valores iniciais):\n", sample + 1);
      printArray(baseArray, tam);
    }
    size_t written = fwrite(baseArray, sizeof(int), (size_t)tam, baseFile);
    if (written != (size_t)tam)
    {
      fprintf(stderr, "Falha ao escrever a amostra %d no arquivo temporario.\n", sample + 1);
      freeArray(baseArray);
      fclose(baseFile);
      free(selected);
      free(totalTimes);
      return 1;
    }
  }
  freeArray(baseArray);

  if (fflush(baseFile) != 0)
  {
    perror("fflush");
    fclose(baseFile);
    free(selected);
    free(totalTimes);
    return 1;
  }

  long long bytesPerSample = (long long)tam * (long long)sizeof(int);
  if (bytesPerSample <= 0)
  {
    fprintf(stderr, "Overflow ao calcular o tamanho da amostra.\n");
    fclose(baseFile);
    free(selected);
    free(totalTimes);
    return 1;
  }
  if (samples > 0 && bytesPerSample > LLONG_MAX / samples)
  {
    fprintf(stderr, "Quantidade de dados excede o limite suportado.\n");
    fclose(baseFile);
    free(selected);
    free(totalTimes);
    return 1;
  }

  printf("\nExecutando benchmarks...\n");
  for (int sample = 0; sample < samples; sample++)
  {
    long long offset = bytesPerSample * (long long)sample;
    printf("Amostra %d/%d:\n", sample + 1, samples);
    for (int algIndex = 0; algIndex < selectedCount; algIndex++)
    {
      const SortAlgorithmEntry *entry = selected[algIndex];
      double elapsed = benchmarkSort(baseFile, tam, offset, entry->function, (tam < 100) && (sample == 0));
      if (elapsed < 0.0)
      {
        fprintf(stderr, "Falha ao executar %s na amostra %d.\n", entry->name, sample + 1);
        fclose(baseFile);
        free(selected);
        free(totalTimes);
        return 1;
      }
      totalTimes[algIndex] += elapsed;
      printf("  [%s] Tempo: %.6f s\n", entry->name, elapsed);
    }
  }

  fclose(baseFile);

  printf("\n=== Resultados medios ===\n");
  for (int i = 0; i < selectedCount; i++)
  {
    double average = totalTimes[i] / (double)samples;
    printf("%s -> media %.6f s\n", selected[i]->name, average);
  }

  free(selected);
  free(totalTimes);
  return 0;
}

static double benchmarkSort(FILE *baseFile, int tam, long long offsetBytes, void (*sortFunction)(int *, int), int printResult)
{
  if (offsetBytes < 0 || offsetBytes > LONG_MAX)
  {
    fprintf(stderr, "Offset fora do intervalo suportado (%lld).\n", offsetBytes);
    return -1.0;
  }

  int *arr = createArray(tam);
  if (arr == NULL)
  {
    fprintf(stderr, "Falha ao alocar memoria para o algoritmo corrente.\n");
    return -1.0;
  }
  if (fseek(baseFile, (long)offsetBytes, SEEK_SET) != 0)
  {
    perror("fseek");
    freeArray(arr);
    return -1.0;
  }
  size_t read = fread(arr, sizeof(int), (size_t)tam, baseFile);
  if (read != (size_t)tam)
  {
    fprintf(stderr, "Falha ao ler a amostra do arquivo temporario (lidos %zu de %d elementos).\n", read, tam);
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
