#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/time.h>
#include "array_random.h"
#include "sum.h"

int main(int argc, char **argv) {
    uint32_t threads_num = 0;
    uint32_t array_size = 0;
    uint32_t seed = 0;

    while (true) {
        int current_optind = optind ? optind : 1;

        static struct option options[] = {
            {"seed", required_argument, 0, 0},
            {"array_size", required_argument, 0, 0},
            {"threads_num", required_argument, 0, 0},
            {0, 0, 0, 0}
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "", options, &option_index);
        if (c == -1) break;

        switch (c) {
            case 0:
                switch (option_index) {
                    case 0:
                        seed = atoi(optarg);
                        if (seed < 0)
                            fprintf(stderr, "Seed must be a non-negative integer.\n");
                        break;
                    case 1:
                        array_size = atoi(optarg);
                        if (array_size <= 0)
                            fprintf(stderr, "Array size must be a positive integer.\n");
                        break;
                    case 2:
                        threads_num = atoi(optarg);
                        if (threads_num <= 0)
                            fprintf(stderr, "Number of threads must be a positive integer.\n");
                        break;
                    default:
                        fprintf(stderr, "Unexpected option index: %d\n", option_index);
                }
                break;
            case '?':
                fprintf(stderr, "Unrecognized option.\n");
                break;
            default:
                fprintf(stderr, "getopt returned unexpected character code: 0%o\n", c);
        }
    }

    if (seed == 0 || array_size == 0 || threads_num == 0) {
        fprintf(stderr, "Usage: %s --seed NUM --array_size NUM --threads_num NUM\n", argv[0]);
        return 1;
    }

    pthread_t *threads = malloc(sizeof(pthread_t) * threads_num);
    struct SumArgs *args = malloc(sizeof(struct SumArgs) * threads_num);
    int *array = malloc(sizeof(int) * array_size);

    GenerateArray(array, array_size, seed);

    struct timeval start_time;
    gettimeofday(&start_time, NULL);

    for (uint32_t i = 0; i < threads_num; i++) {
        args[i].array = array;
        args[i].begin = i * (array_size / threads_num);
        args[i].end = (i == threads_num - 1) ? array_size : (i + 1) * (array_size / threads_num);
        args[i].partial_sum = 0;

        if (pthread_create(&threads[i], NULL, ThreadSum, (void *)&args[i])) {
            fprintf(stderr, "Error: pthread_create failed!\n");
            free(array);
            free(threads);
            free(args);
            return 1;
        }
    }

    int total_sum = 0;
    for (uint32_t i = 0; i < threads_num; i++) {
        pthread_join(threads[i], NULL);
        total_sum += args[i].partial_sum;
    }

    struct timeval finish_time;
    gettimeofday(&finish_time, NULL);

    double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
    elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

    free(array);
    free(threads);
    free(args);

    printf("Total: %d\n", total_sum);
    printf("Elapsed time: %.2f ms\n", elapsed_time);
    fflush(NULL);

    return 0;
}