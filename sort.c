#include "sort.h"

int arrayMax(int *array, int size) {
    int max = array[0];
    for(int i = 1; i < size; ++i)
        max = array[i] > max ? array[i] : max;
    return max;
}

void countSort(int *array, int size, int exponent) {
    //int output[size];
	int *output = malloc(sizeof(int) * size);
    int i, count[10] = {0};

    for(i = 0; i < size; ++i)
        ++count[(array[i]/exponent)%10];
    for(i = 1; i < 10; ++i)
        count[i] += count[i-1];
    for(i = size-1; i >= 0; --i) {
        output[count[(array[i]/exponent)%10] - 1] = array[i];
        --count[(array[i]/exponent)%10];
    }
    for(i = 0; i < size; ++i)
        array[i] = output[i];

	free(output);
}

void radixSort(int *array, int size) {
    int max = arrayMax(array, size);
    for(int exponent = 1; max/exponent > 0; exponent *= 10)
        countSort(array, size, exponent);
}

void printArray(int *array, int size) {
    printf(" \n Array: ");
    for(int i = 0; i < size; ++i)
        printf("%d ", array[i]);
    printf("\n");
}
