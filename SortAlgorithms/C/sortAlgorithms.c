#include <stdio.h>
#include <stdlib.h>

#include "sortAlgorithms.h"

void quickSortRecursive(int arr[], int low, int high);
int partition(int arr[], int low, int high);
int ramdomizedPartition(int arr[], int low, int high);

void bubbleSort(int* arr, int n) {
    int i, j, temp;
    for (i = 0; i < n-1; i++) {
        for (j = 0; j < n-i-1; j++) {
            if (arr[j] > arr[j+1]) {
                temp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = temp;
            }
        }
    }
}

void selectionSort(int arr[], int n) {
    int i, j, minIndex, temp;
    for (i = 0; i < n - 1; i++) {
        minIndex = i;
        for (j = i + 1; j < n; j++) {
            if (arr[j] < arr[minIndex]) {
                minIndex = j;
            }
        }
        if (minIndex != i) {
            temp = arr[i];
            arr[i] = arr[minIndex];
            arr[minIndex] = temp;
        }
    }
}

void insertionSort(int arr[], int n) {
    int i, j, key;
    for (i = 1; i < n; i++) {
        key = arr[i];
        j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j+1] = arr[j];
            j--;
        }
        arr[j+1] = key;
    }
}

void quickSort(int arr[], int n) {
    quickSortRecursive(arr, 0, n-1);
}

void quickSortRecursive(int arr[], int low, int high) {
    int pivot;
    if (low < high) {
        pivot = ramdomizedPartition(arr, low, high);
        quickSortRecursive(arr, low, pivot - 1);
        quickSortRecursive(arr, pivot + 1, high);
    }
}

int partition(int arr[], int low, int high) {
    int pivot = arr[high];
    int i = low;
    int temp;
    for (int j = low; j < high; j++) {
        if (arr[j] <= pivot) {
            temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
            i++;
        }
    }
    temp = arr[i];
    arr[i] = arr[high];
    arr[high] = temp;
    return i;
}

int ramdomizedPartition(int arr[], int low, int high) {
    int i = low + (rand() % (high - low + 1));
    int temp = arr[i];
    arr[i] = arr[high];
    arr[high] = temp;
    return partition(arr, low, high);
}


