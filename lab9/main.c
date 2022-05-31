#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "utils.h"

int main() {
    for (int i = 0; i < ELVES_NEEDED; i++) eid_waiting[i] = -1;

    int *e_id = calloc(TOTAL_FACTORY_SLAVES, sizeof(int));
    int *r_id = calloc(TOTAL_REINDEER, sizeof(int));

    pthread_t *e_threads = calloc(TOTAL_FACTORY_SLAVES, sizeof(pthread_t));
    pthread_t *r_threads = calloc(TOTAL_REINDEER, sizeof(pthread_t));
    pthread_t s_thread;
    pthread_create(&s_thread, NULL, *santa_work, NULL);

    for (int i = 0; i < TOTAL_FACTORY_SLAVES; i++) {
        e_id[i] = i;
        pthread_create(&e_threads[i], NULL, &elf_work, &e_id[i]);
    }
    for (int i = 0; i < TOTAL_REINDEER; i++) {
        r_id[i] = i;
        pthread_create(&r_threads[i], NULL, &reindeer_work, &r_id[i]);
    }

    for (int i = 0; i < TOTAL_FACTORY_SLAVES; i++) {
        pthread_join(e_threads[i], NULL);
    }
    for (int i = 0; i < TOTAL_REINDEER; i++) {
        pthread_join(r_threads[i], NULL);
    }

    free(e_id);
    free(e_threads);
    free(r_id);
    free(r_threads);

    pthread_cond_destroy(&santa_cond);
    pthread_cond_destroy(&reindeer_wait_cond);
    pthread_cond_destroy(&elf_wait_cond);

    pthread_mutex_destroy(&santa_mutex);
    pthread_mutex_destroy(&reindeer_mutex);
    pthread_mutex_destroy(&reindeer_wait_mutex);
    pthread_mutex_destroy(&elf_mutex);
    pthread_mutex_destroy(&elf_wait_mutex);

    return 0;
}