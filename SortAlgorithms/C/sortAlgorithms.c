#include <stdio.h>
#include <stdlib.h>
#ifdef _OPENMP
#include <omp.h>
#endif

#include "sortAlgorithms.h"

typedef struct
{
    int leftEnd;
    int rightStart;
} PartitionBounds;

static void quickSortRecursive(int arr[], int low, int high);
static PartitionBounds partitionThreeWay(int arr[], int low, int high);
static void insertionSortRange(int arr[], int left, int right);
static inline void swapIntegers(int *a, int *b);
static int medianOfThree(int arr[], int low, int high);
static void mergeSortRecursive(int arr[], int temp[], int left, int right);
static void merge(int arr[], int temp[], int left, int mid, int right);
static void buildMaxHeap(int arr[], int n);
static void buildMaxHeapRecursive(int arr[], int n, int index, int depth);
static void heapify(int arr[], int n, int i);

#define MERGE_SORT_PARALLEL_THRESHOLD 2048
#define HEAP_BUILD_PARALLEL_DEPTH 4
#define QUICK_SORT_PARALLEL_THRESHOLD 1024
#define QUICK_SORT_INSERTION_THRESHOLD 32

void bubbleSort(int *arr, int n)
{
    int i, j, temp;
    for (i = 0; i < n - 1; i++)
    {
        for (j = 0; j < n - i - 1; j++)
        {
            if (arr[j] > arr[j + 1])
            {
                temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

void selectionSort(int arr[], int n)
{
    int i, j, minIndex, temp;
    for (i = 0; i < n - 1; i++)
    {
        minIndex = i;
        for (j = i + 1; j < n; j++)
        {
            if (arr[j] < arr[minIndex])
            {
                minIndex = j;
            }
        }
        if (minIndex != i)
        {
            temp = arr[i];
            arr[i] = arr[minIndex];
            arr[minIndex] = temp;
        }
    }
}

void insertionSort(int arr[], int n)
{
    int i, j, key;
    for (i = 1; i < n; i++)
    {
        key = arr[i];
        j = i - 1;
        while (j >= 0 && arr[j] > key)
        {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

void quickSort(int arr[], int n)
{
    if (n <= 1)
    {
        return;
    }
#ifdef _OPENMP
#pragma omp parallel
    {
#pragma omp single nowait
        quickSortRecursive(arr, 0, n - 1);
    }
#else
    quickSortRecursive(arr, 0, n - 1);
#endif
}

static void quickSortRecursive(int arr[], int low, int high)
{
    while (low < high)
    {
        if ((high - low + 1) <= QUICK_SORT_INSERTION_THRESHOLD)
        {
            insertionSortRange(arr, low, high);
            return;
        }
        PartitionBounds bounds = partitionThreeWay(arr, low, high);
        int leftHigh = bounds.leftEnd;
        int rightLow = bounds.rightStart;
#ifdef _OPENMP
        if ((high - low) >= QUICK_SORT_PARALLEL_THRESHOLD)
        {
#pragma omp taskgroup
            {
                if (leftHigh >= low)
                {
#pragma omp task shared(arr) firstprivate(low, leftHigh)
                    quickSortRecursive(arr, low, leftHigh);
                }
                if (rightLow <= high)
                {
#pragma omp task shared(arr) firstprivate(rightLow, high)
                    quickSortRecursive(arr, rightLow, high);
                }
            }
            return;
        }
#endif
        if (leftHigh >= low)
        {
            quickSortRecursive(arr, low, leftHigh);
        }
        low = rightLow;
    }
}

void mergeSort(int arr[], int n)
{
    if (n <= 1)
    {
        return;
    }
    int *temp = (int *)malloc(n * sizeof(int));
    if (!temp)
    {
        return;
    }
#ifdef _OPENMP
#pragma omp parallel
    {
#pragma omp single nowait
        mergeSortRecursive(arr, temp, 0, n - 1);
    }
#else
    mergeSortRecursive(arr, temp, 0, n - 1);
#endif
    free(temp);
}

static void mergeSortRecursive(int arr[], int temp[], int left, int right)
{
    if (left >= right)
    {
        return;
    }
    int mid = left + (right - left) / 2;
#ifdef _OPENMP
    if ((right - left) >= MERGE_SORT_PARALLEL_THRESHOLD)
    {
#pragma omp task shared(arr, temp) firstprivate(left, mid)
        mergeSortRecursive(arr, temp, left, mid);
#pragma omp task shared(arr, temp) firstprivate(mid, right)
        mergeSortRecursive(arr, temp, mid + 1, right);
#pragma omp taskwait
    }
    else
    {
        mergeSortRecursive(arr, temp, left, mid);
        mergeSortRecursive(arr, temp, mid + 1, right);
    }
#else
    mergeSortRecursive(arr, temp, left, mid);
    mergeSortRecursive(arr, temp, mid + 1, right);
#endif
    merge(arr, temp, left, mid, right);
}

static void merge(int arr[], int temp[], int left, int mid, int right)
{
    int i = left;
    int j = mid + 1;
    int k = left;
    while (i <= mid && j <= right)
    {
        if (arr[i] <= arr[j])
        {
            temp[k++] = arr[i++];
        }
        else
        {
            temp[k++] = arr[j++];
        }
    }
    while (i <= mid)
    {
        temp[k++] = arr[i++];
    }
    while (j <= right)
    {
        temp[k++] = arr[j++];
    }
    for (int idx = left; idx <= right; idx++)
    {
        arr[idx] = temp[idx];
    }
}

void heapSort(int arr[], int n)
{
    if (n <= 1)
    {
        return;
    }
    buildMaxHeap(arr, n);
    for (int i = n - 1; i > 0; i--)
    {
        int temp = arr[0];
        arr[0] = arr[i];
        arr[i] = temp;
        heapify(arr, i, 0);
    }
}

static void buildMaxHeap(int arr[], int n)
{
#ifdef _OPENMP
#pragma omp parallel
    {
#pragma omp single nowait
        buildMaxHeapRecursive(arr, n, 0, 0);
    }
#else
    buildMaxHeapRecursive(arr, n, 0, 0);
#endif
}

static void buildMaxHeapRecursive(int arr[], int n, int index, int depth)
{
    if (index >= n)
    {
        return;
    }
    int left = 2 * index + 1;
    int right = left + 1;
#ifdef _OPENMP
    if (depth < HEAP_BUILD_PARALLEL_DEPTH)
    {
        if (left < n)
        {
            int child = left;
            int childDepth = depth + 1;
#pragma omp task shared(arr, n) firstprivate(child, childDepth)
            buildMaxHeapRecursive(arr, n, child, childDepth);
        }
        if (right < n)
        {
            int child = right;
            int childDepth = depth + 1;
#pragma omp task shared(arr, n) firstprivate(child, childDepth)
            buildMaxHeapRecursive(arr, n, child, childDepth);
        }
#pragma omp taskwait
    }
    else
    {
        if (left < n)
        {
            buildMaxHeapRecursive(arr, n, left, depth + 1);
        }
        if (right < n)
        {
            buildMaxHeapRecursive(arr, n, right, depth + 1);
        }
    }
#else
    if (left < n)
    {
        buildMaxHeapRecursive(arr, n, left, depth + 1);
    }
    if (right < n)
    {
        buildMaxHeapRecursive(arr, n, right, depth + 1);
    }
#endif
    heapify(arr, n, index);
}

static void heapify(int arr[], int n, int i)
{
    int largest = i;
    int left = 2 * i + 1;
    int right = left + 1;
    if (left < n && arr[left] > arr[largest])
    {
        largest = left;
    }
    if (right < n && arr[right] > arr[largest])
    {
        largest = right;
    }
    if (largest != i)
    {
        int temp = arr[i];
        arr[i] = arr[largest];
        arr[largest] = temp;
        heapify(arr, n, largest);
    }
}
static PartitionBounds partitionThreeWay(int arr[], int low, int high)
{
    PartitionBounds bounds = {low - 1, high + 1};
    if (low >= high)
    {
        return bounds;
    }
    int pivot = medianOfThree(arr, low, high);
    int lt = low;
    int i = low;
    int gt = high;
    while (i <= gt)
    {
        if (arr[i] < pivot)
        {
            swapIntegers(&arr[i], &arr[lt]);
            lt++;
            i++;
        }
        else if (arr[i] > pivot)
        {
            swapIntegers(&arr[i], &arr[gt]);
            gt--;
        }
        else
        {
            i++;
        }
    }
    bounds.leftEnd = lt - 1;
    bounds.rightStart = gt + 1;
    return bounds;
}

static void insertionSortRange(int arr[], int left, int right)
{
    for (int i = left + 1; i <= right; i++)
    {
        int key = arr[i];
        int j = i - 1;
        while (j >= left && arr[j] > key)
        {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

static inline void swapIntegers(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

static int medianOfThree(int arr[], int low, int high)
{
    int mid = low + (high - low) / 2;
    if (arr[low] > arr[mid])
    {
        swapIntegers(&arr[low], &arr[mid]);
    }
    if (arr[mid] > arr[high])
    {
        swapIntegers(&arr[mid], &arr[high]);
    }
    if (arr[low] > arr[mid])
    {
        swapIntegers(&arr[low], &arr[mid]);
    }
    return arr[mid];
}
