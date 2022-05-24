#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <string.h>

#include "utils.h"

int main(int argc, char **argv) {
    if (argc != 5) {
        printf("Error: Incorrect number of arguments.\n");
        exit(1);
    }
    no_of_threads = atoi(argv[1]);
    char *method = argv[2];
    char *input_file = argv[3];
    char *output_file = argv[4];

    get_input_img(input_file);
    negated_array = calloc(height, sizeof(int *));
    for (int i = 0; i < height; i++) {
        negated_array[i] = calloc(width, sizeof(int));
    }
    int *th_array = calloc(no_of_threads, sizeof(int));
    pthread_t *threads = calloc(no_of_threads, sizeof(pthread_t));
    struct timeval start;
    struct timeval stop;
    gettimeofday(&start, NULL);

    for (int i = 0; i < no_of_threads; i++) {
        th_array[i] = i;
        if (strcmp(method, "blocks") == 0) {
            pthread_create(&threads[i], NULL, &process_img_blocks, &th_array[i]);
        } else if (strcmp(method, "numbers") == 0) {
            pthread_create(&threads[i], NULL, &process_img_numbers, &th_array[i]);
        } else {
            printf("Error: Method unrecognized! Try [blocks]|[numbers].");
        }
    }

    double *thread_time = get_thread_information(threads);
    gettimeofday(&stop, NULL);
    get_logs_threads(start, stop, thread_time, method);
    save_output_img(output_file);

    for (int i = 0; i < height; i++) {
        free(img_array[i]);
        free(negated_array[i]);
    }
    free(img_array);
    free(negated_array);

    return 0;
}