#ifndef SUM_H
#define SUM_H

struct SumArgs {
  int *array;
  int begin;
  int end;
  int partial_sum;
};

int Sum(const struct SumArgs *args);
void *ThreadSum(void *args);

#endif