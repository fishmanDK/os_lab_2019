#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

#define MAX_PNUM 1000
int child_pids[MAX_PNUM];
int pnum = 0;

void timeout_handler(int signum) {
    for (int i = 0; i < pnum; i++) {
        if (child_pids[i] > 0) {
            kill(child_pids[i], SIGKILL);
        }
    }
    printf("\nTimeout reached. Child processes killed.\n");
}

int main(int argc, char **argv) {
    int seed = -1;
    int array_size = -1;
    int timeout = 0;
    bool with_files = false;

    while (true) {
        static struct option options[] = {
            {"seed", required_argument, 0, 0},
            {"array_size", required_argument, 0, 0},
            {"pnum", required_argument, 0, 0},
            {"by_files", no_argument, 0, 'f'},
            {"timeout", required_argument, 0, 0},
            {0, 0, 0, 0}};

        int option_index = 0;
        int c = getopt_long(argc, argv, "f", options, &option_index);
        if (c == -1) break;

        switch (c) {
            case 0:
                switch (option_index) {
                    case 0:
                        seed = atoi(optarg);
                        break;
                    case 1:
                        array_size = atoi(optarg);
                        break;
                    case 2:
                        pnum = atoi(optarg);
                        break;
                    case 3:
                        with_files = true;
                        break;
                    case 4:
                        timeout = atoi(optarg);
                        break;
                    default:
                        printf("Index %d is out of options\n", option_index);
                }
                break;
            case 'f':
                with_files = true;
                break;
            case '?':
                break;
            default:
                printf("getopt returned unknown option\n");
        }
    }

    if (optind < argc) {
        printf("Has at least one no-option argument\n");
        return 1;
    }

    if (seed == -1 || array_size == -1 || pnum <= 0 || pnum > MAX_PNUM) {
        printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" [--timeout num]\n", argv[0]);
        return 1;
    }

    int *array = malloc(sizeof(int) * array_size);
    GenerateArray(array, array_size, seed);

    int pipefd[pnum][2];
    if (!with_files) {
        for (int i = 0; i < pnum; ++i) {
            if (pipe(pipefd[i]) == -1) {
                perror("pipe");
                exit(1);
            }
        }
    }

    if (timeout > 0) {
        signal(SIGALRM, timeout_handler);
        alarm(timeout);
    }

    struct timeval start_time;
    gettimeofday(&start_time, NULL);

    for (int i = 0; i < pnum; i++) {
        pid_t child_pid = fork();
        if (child_pid >= 0) {
            child_pids[i] = child_pid;
            if (child_pid == 0) {
                int start = i * array_size / pnum;
                int end = (i == pnum - 1) ? array_size : (i + 1) * array_size / pnum;
                struct MinMax local_min_max = GetMinMax(array, start, end);

                if (with_files) {
                    char filename[64];
                    sprintf(filename, "tmp_min_max_%d.txt", i);
                    FILE *fp = fopen(filename, "w");
                    fwrite(&local_min_max, sizeof(struct MinMax), 1, fp);
                    fclose(fp);
                } else {
                    write(pipefd[i][1], &local_min_max, sizeof(struct MinMax));
                    close(pipefd[i][1]);
                }

                free(array);
                exit(0);
            }
        } else {
            printf("Fork failed!\n");
            return 1;
        }
    }
    int active_child_processes = pnum;
    while (active_child_processes > 0) {
        int status;
        pid_t pid = waitpid(-1, &status, WNOHANG);
        if (pid > 0) {
            active_child_processes--;
        } else {
            usleep(100000); // 0.1 секунды
        }
    }

    struct MinMax final_min_max;
    final_min_max.min = INT_MAX;
    final_min_max.max = INT_MIN;

    for (int i = 0; i < pnum; i++) {
        struct MinMax local_min_max;

        if (with_files) {
            char filename[64];
            sprintf(filename, "tmp_min_max_%d.txt", i);
            FILE *fp = fopen(filename, "r");
            fread(&local_min_max, sizeof(struct MinMax), 1, fp);
            fclose(fp);
        } else {
            read(pipefd[i][0], &local_min_max, sizeof(struct MinMax));
            close(pipefd[i][0]);
        }

        if (local_min_max.min < final_min_max.min)
            final_min_max.min = local_min_max.min;
        if (local_min_max.max > final_min_max.max)
            final_min_max.max = local_min_max.max;
    }

    struct timeval finish_time;
    gettimeofday(&finish_time, NULL);

    double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
    elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

    free(array);

    printf("Min: %d\n", final_min_max.min);
    printf("Max: %d\n", final_min_max.max);
    printf("Elapsed time: %fms\n", elapsed_time);

    return 0;
}