#ifndef LAB9_UTILS_H
#define LAB9_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define TOTAL_DELIVERIES 3
#define TOTAL_REINDEER 9
#define TOTAL_FACTORY_SLAVES 10
#define ELVES_NEEDED 3

pthread_mutex_t santa_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t reindeer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t reindeer_wait_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t elf_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t elf_wait_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t santa_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t reindeer_wait_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t elf_wait_cond = PTHREAD_COND_INITIALIZER;

int elves_now_waiting = 0;
int eid_waiting[3];
int reindeer_now_waiting = 0;
int reindeer_can_move = 1;

void *santa_work(void *args);
void *elf_work(void *arg);
void *reindeer_work(void *arg);
int random_operation_time(int low, int high);

void *santa_work(void *args) {
    srand(time(NULL));

    int already_delivered = 0;
    while (already_delivered < TOTAL_DELIVERIES) {
        pthread_mutex_lock(&santa_mutex);

        while (reindeer_now_waiting < TOTAL_REINDEER && elves_now_waiting < ELVES_NEEDED) {
            printf("Mikołaj: śpię..\n");
            pthread_cond_wait(&santa_cond, &santa_mutex);
        }

        pthread_mutex_unlock(&santa_mutex);

        printf("Mikołaj: budzę się..\n");

        pthread_mutex_lock(&reindeer_mutex);

        if (reindeer_now_waiting == TOTAL_REINDEER) {
            printf("Mikołaj: dostarczam zabawki..\n");
            sleep(random_operation_time(2, 5));
            already_delivered++;
            reindeer_now_waiting = 0;
            pthread_mutex_lock(&reindeer_wait_mutex);
            reindeer_can_move = 1;
            pthread_cond_broadcast(&reindeer_wait_cond);
            pthread_mutex_unlock(&reindeer_wait_mutex);
        }

        pthread_mutex_unlock(&reindeer_mutex);

        pthread_mutex_lock(&elf_mutex);

        if (elves_now_waiting == ELVES_NEEDED) {
            pthread_mutex_lock(&elf_wait_mutex);

            printf("Mikołaj: rozwiązuję problemy elfów..\t");
            for (int i = 0; i < ELVES_NEEDED; i++) {
                printf("%d ", eid_waiting[i]);
                eid_waiting[i] = -1;
            }
            printf("\n");
            sleep(random_operation_time(1, 3));
            elves_now_waiting = 0;
            pthread_cond_broadcast(&elf_wait_cond);

            pthread_mutex_unlock(&elf_wait_mutex);
        }

        pthread_mutex_unlock(&elf_mutex);

        printf("Mikołaj: zasypiam..\n");
    }
    exit(EXIT_SUCCESS);
}

void *elf_work(void *arg) {
    int ID = *((int *) arg);
    srand(ID);

    while (1) {
        sleep(random_operation_time(2, 5));

        pthread_mutex_lock(&elf_wait_mutex);

        while (elves_now_waiting == ELVES_NEEDED) {
            printf("Elf: czeka na powrót elfów, ID: %d\n", ID);
            pthread_cond_wait(&elf_wait_cond, &elf_wait_mutex);
        }

        pthread_mutex_unlock(&elf_wait_mutex);

        pthread_mutex_lock(&elf_mutex);

        if (elves_now_waiting < ELVES_NEEDED) {
            eid_waiting[elves_now_waiting] = ID;
            elves_now_waiting++;
            printf("Elf: czeka %d elfów na Mikołaja, ID: %d\n", elves_now_waiting, ID);

            if (elves_now_waiting == ELVES_NEEDED) {
                printf("Elf: wybudzam Mikołaja, ID: %d\n", ID);
                pthread_mutex_lock(&santa_mutex);

                pthread_cond_broadcast(&santa_cond);

                pthread_mutex_unlock(&santa_mutex);
            }
        }

        pthread_mutex_unlock(&elf_mutex);
    }
}

void *reindeer_work(void *arg) {
    int ID = *((int *) arg);
    srand(ID);

    while (1) {
        pthread_mutex_lock(&reindeer_wait_mutex);

        while (!reindeer_can_move) {
            pthread_cond_wait(&reindeer_wait_cond, &reindeer_wait_mutex);
        }

        pthread_mutex_unlock(&reindeer_wait_mutex);

        sleep(random_operation_time(5, 10));

        pthread_mutex_lock(&reindeer_mutex);

        reindeer_now_waiting++;
        printf("Renifer: czeka %d reniferów na Mikołaja, ID: %d\n", reindeer_now_waiting, ID);
        reindeer_can_move = 0;

        if (reindeer_now_waiting == TOTAL_REINDEER) {
            printf("Refinfer: wybudzam Mikołaja, ID: %d\n", ID);
            pthread_mutex_lock(&santa_mutex);

            pthread_cond_broadcast(&santa_cond);

            pthread_mutex_unlock(&santa_mutex);
        }

        pthread_mutex_unlock(&reindeer_mutex);

        sleep(random_operation_time(2, 5));
    }
}

int random_operation_time(int low, int high) {
    return random() % (high - low + 1) + low;
}

#endif //LAB9_UTILS_H
