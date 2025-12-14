#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
static void heapSortSequentialRange(int arr[], int start, int length);
static void heapifyDownRange(int arr[], int start, int length, int root);
static void parallelMergeSegments(int arr[], int temp[], const int offsets[], int segmentCount);
static void mergeSegmentsRecursive(int *src, int *dst, const int offsets[], int leftSegment, int rightSegment);
static void mergeSortedSubarrays(const int src[], int dst[], int leftStart, int leftEnd, int rightEnd);

#define MERGE_SORT_PARALLEL_THRESHOLD 2048
#define QUICK_SORT_PARALLEL_THRESHOLD 1024
#define QUICK_SORT_INSERTION_THRESHOLD 32
#define HEAP_MIN_CHUNK_SIZE 16384
#define HEAP_MAX_SEGMENTS 64
#define HEAP_MERGE_PARALLEL_THRESHOLD 32768

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

static void heapSortSequentialRange(int arr[], int start, int length)
{
    if (length <= 1)
    {
        return;
    }
    for (int i = (length / 2) - 1; i >= 0; --i)
    {
        heapifyDownRange(arr, start, length, i);
    }
    for (int end = length - 1; end > 0; --end)
    {
        swapIntegers(&arr[start], &arr[start + end]);
        heapifyDownRange(arr, start, end, 0);
    }
}

static void heapifyDownRange(int arr[], int start, int length, int root)
{
    int current = root;
    while (1)
    {
        int left = 2 * current + 1;
        int right = left + 1;
        int largest = current;
        if (left < length && arr[start + left] > arr[start + largest])
        {
            largest = left;
        }
        if (right < length && arr[start + right] > arr[start + largest])
        {
            largest = right;
        }
        if (largest == current)
        {
            break;
        }
        swapIntegers(&arr[start + current], &arr[start + largest]);
        current = largest;
    }
}

static void parallelMergeSegments(int arr[], int temp[], const int offsets[], int segmentCount)
{
    if (segmentCount <= 1)
    {
        return;
    }
    int totalLength = offsets[segmentCount];
    memcpy(temp, arr, (size_t)totalLength * sizeof(int));
#ifdef _OPENMP
#pragma omp parallel
    {
#pragma omp single nowait
        mergeSegmentsRecursive(temp, arr, offsets, 0, segmentCount);
    }
#else
    mergeSegmentsRecursive(temp, arr, offsets, 0, segmentCount);
#endif
}

static void mergeSegmentsRecursive(int *src, int *dst, const int offsets[], int leftSegment, int rightSegment)
{
    if ((rightSegment - leftSegment) <= 1)
    {
        int start = offsets[leftSegment];
        int end = offsets[rightSegment];
        if (start < end)
        {
            memcpy(dst + start, src + start, (size_t)(end - start) * sizeof(int));
        }
        return;
    }
    int mid = leftSegment + (rightSegment - leftSegment) / 2;
#ifdef _OPENMP
    int span = offsets[rightSegment] - offsets[leftSegment];
    if (span >= HEAP_MERGE_PARALLEL_THRESHOLD)
    {
#pragma omp task shared(src, dst, offsets) firstprivate(leftSegment, mid)
        mergeSegmentsRecursive(dst, src, offsets, leftSegment, mid);
#pragma omp task shared(src, dst, offsets) firstprivate(mid, rightSegment)
        mergeSegmentsRecursive(dst, src, offsets, mid, rightSegment);
#pragma omp taskwait
    }
    else
    {
        mergeSegmentsRecursive(dst, src, offsets, leftSegment, mid);
        mergeSegmentsRecursive(dst, src, offsets, mid, rightSegment);
    }
#else
    mergeSegmentsRecursive(dst, src, offsets, leftSegment, mid);
    mergeSegmentsRecursive(dst, src, offsets, mid, rightSegment);
#endif
    mergeSortedSubarrays(src, dst, offsets[leftSegment], offsets[mid], offsets[rightSegment]);
}

static void mergeSortedSubarrays(const int src[], int dst[], int leftStart, int leftEnd, int rightEnd)
{
    int i = leftStart;
    int j = leftEnd;
    int k = leftStart;
    while (i < leftEnd && j < rightEnd)
    {
        if (src[i] <= src[j])
        {
            dst[k++] = src[i++];
        }
        else
        {
            dst[k++] = src[j++];
        }
    }
    while (i < leftEnd)
    {
        dst[k++] = src[i++];
    }
    while (j < rightEnd)
    {
        dst[k++] = src[j++];
    }
}

void heapSort(int arr[], int n)
{
    if (n <= 1)
    {
        return;
    }

    int estimatedSegments;
#ifdef _OPENMP
    estimatedSegments = omp_get_max_threads();
    if (estimatedSegments < 1)
    {
        estimatedSegments = 1;
    }
#else
    estimatedSegments = 1;
#endif

    int maxSegmentsBySize = n / HEAP_MIN_CHUNK_SIZE;
    if (maxSegmentsBySize == 0)
    {
        maxSegmentsBySize = 1;
    }
    if (estimatedSegments > maxSegmentsBySize)
    {
        estimatedSegments = maxSegmentsBySize;
    }
    if (estimatedSegments > HEAP_MAX_SEGMENTS)
    {
        estimatedSegments = HEAP_MAX_SEGMENTS;
    }

    int *segmentOffsets = (int *)malloc((estimatedSegments + 1) * sizeof(int));
    if (segmentOffsets == NULL)
    {
        heapSortSequentialRange(arr, 0, n);
        return;
    }

    int chunkSize = (n + estimatedSegments - 1) / estimatedSegments;
    int actualSegments = 0;
    int start = 0;
    while (start < n && actualSegments < estimatedSegments)
    {
        segmentOffsets[actualSegments++] = start;
        start += chunkSize;
    }
    segmentOffsets[actualSegments] = n;
    estimatedSegments = actualSegments;

#ifdef _OPENMP
#pragma omp parallel for schedule(dynamic)
#endif
    for (int i = 0; i < estimatedSegments; ++i)
    {
        int segStart = segmentOffsets[i];
        int segLength = segmentOffsets[i + 1] - segmentOffsets[i];
        heapSortSequentialRange(arr, segStart, segLength);
    }

    if (estimatedSegments == 1)
    {
        free(segmentOffsets);
        return;
    }

    int *temp = (int *)malloc((size_t)n * sizeof(int));
    if (temp == NULL)
    {
        heapSortSequentialRange(arr, 0, n);
        free(segmentOffsets);
        return;
    }

    parallelMergeSegments(arr, temp, segmentOffsets, estimatedSegments);

    free(temp);
    free(segmentOffsets);
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
