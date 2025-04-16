#include "sum.h"

#include <stdio.h>
#include <stdlib.h>

int Sum(const struct SumArgs *args) {
    int sum = 0;
    for (int i = args->begin; i < args->end; i++) {
        sum += args->array[i]; // Добавляем текущий элемент к сумме
    }
    return sum; 
}

void *ThreadSum(void *args) {
    struct SumArgs *a = (struct SumArgs *)args;
    a->partial_sum = 0;
    for (int i = a->begin; i < a->end; ++i) {
        a->partial_sum += a->array[i];
    }
    return NULL;
}