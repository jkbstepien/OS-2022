#ifndef LAB8_UTILS_H
#define LAB8_UTILS_H

#define ROW_LENGTH 256

int width;
int height;
int no_of_threads;
int **img_array;
int **negated_array;

double *get_thread_information(pthread_t *threads) {
    double *thread_times = calloc(no_of_threads, sizeof(double));

    for (int i = 0; i < no_of_threads; ++i) {
        double *th;

        if (pthread_join(threads[i], (void **) &th) != 0) {
            fprintf(stderr, "Error @get_thread_information.\n");
            exit(1);
        }

        thread_times[i] = *th;
    }

    return thread_times;
}

void get_logs_threads(struct timeval start, struct timeval stop, double *th_time, char *method_used) {
    FILE *fp = fopen("Times.txt", "a");

    fprintf(fp, "\noption: %s\n", method_used);
    fprintf(fp, "threads: %d\n", no_of_threads);

    for (int i = 0; i < no_of_threads; ++i) {
        fprintf(fp, "\tthread no. %d - %lf\n", i, th_time[i]);
    }

    double *th = calloc(1, sizeof(double));
    *th = (double) ((stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);

    printf("\t---> operation time: %lf\n", *th);

    fprintf(fp, "total: %lf\n\n", *th);
}

void *process_img_blocks(void *arg) {
    struct timeval start;
    struct timeval stop;
    gettimeofday(&start, NULL);

    int curr_index = *((int *) arg);
    int high = width * curr_index / no_of_threads;
    int low = width * (curr_index + 1) / no_of_threads;
    int curr_field;

    for (int i = 0; i < height; i++) {
        for (int j = high; j < low; j++) {
            curr_field = img_array[i][j];
            negated_array[i][j] = 255 - curr_field;
        }
    }

    gettimeofday(&stop, NULL);
    double *t = calloc(1, sizeof(double));
    *t = (double) ((stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);

    pthread_exit(t);
}

void *process_img_numbers(void *arguments) {
    struct timeval start;
    struct timeval stop;
    gettimeofday(&start, NULL);

    int curr_index = *((int *) arguments);
    int high = 256 * curr_index / no_of_threads;
    int low = 256 * (curr_index + 1) / no_of_threads;
    int curr_field;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            curr_field = img_array[i][j];
            if (curr_field >= high && curr_field < low) {
                negated_array[i][j] = 255 - curr_field;
            }
        }
    }

    gettimeofday(&stop, NULL);
    double *t = calloc(1, sizeof(double));
    *t = (double) ((stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);

    pthread_exit(t);
}

void get_input_img(char *f_name) {
    FILE *file = fopen(f_name, "r");
    if (file == NULL) {
        printf("Error @get_input_img.\n");
        exit(1);
    }
    char *buffer = calloc(ROW_LENGTH, sizeof(char));
    fgets(buffer, ROW_LENGTH, file);
    fgets(buffer, ROW_LENGTH, file);
    sscanf(buffer, "%d %d\n", &width, &height);
    fgets(buffer, ROW_LENGTH, file);
    img_array = calloc(height, sizeof(int *));
    for (int i = 0; i < height; i++) {
        img_array[i] = calloc(width, sizeof(int));
    }

    int p;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            fscanf(file, "%d", &p);
            img_array[i][j] = p;
        }
    }
    fclose(file);
}

void save_output_img(char *f_name) {
    FILE *pFile = fopen(f_name, "w");
    if (pFile == NULL) {
        printf("Error @save_output_img.\n");
        exit(1);
    }
    fprintf(pFile, "P2\n");
    fprintf(pFile, "%d %d\n", width, height);
    fprintf(pFile, "255\n");
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            fprintf(pFile, "%d ", negated_array[i][j]);
        }
        fprintf(pFile, "\n");
    }
    fclose(pFile);
}

#endif //LAB8_UTILS_H
