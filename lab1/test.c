//
// Created by jkbstepien on 14/03/2022.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#ifdef LIB_DYNAMIC
#include <dlfcn.h>
#else
#include "library.h"
#endif


#ifdef PERFORMANCE_TESTS
#include <sys/times.h>
#include <stdint.h>
#include <unistd.h>
clock_t clock_t_begin, clock_t_end;
struct tms times_start_buffer, times_end_buffer;

void start_timer(){
	clock_t_begin = times(&times_start_buffer);
}

void stop_timer(){
	clock_t_end = times(&times_end_buffer);
}

double calc_time(clock_t s, clock_t e) {
    return ((double) (e - s) / (double) sysconf(_SC_CLK_TCK));
}

void print_times(const char* operation){
	printf("%20s real %.3fs user %.3fs sys %.3fs\n",
        operation,
        calc_time(clock_t_begin, clock_t_end),
        calc_time(times_start_buffer.tms_cutime, times_end_buffer.tms_cutime),
        calc_time(times_start_buffer.tms_cstime, times_end_buffer.tms_cstime));
}
#endif

int main(int argc, char** argv){
#ifdef LIB_DYNAMIC
    void *handle = dlopen("../zad1/library.so", RTLD_LAZY);
        if (!handle) {
            fprintf(stderr, "Cannot load library.so\n");
            exit(4);
        }

        //void create_table(int size);
        void (*create_table)(int) = (void (*)(int)) dlsym(handle, "create_table");
        //void count(char** filenames, int len);
        void (*count)(char**, int) = (void (*) (char**, int)) dlsym(handle, "count");
        //int read_table_from_file();
        int (*read_table_from_file)(void) = (int(*)(void)) dlsym(handle, "read_table_from_file");
        //void free_table();
        void (*free_table)(void) = (void(*)(void)) dlsym(handle, "free_table");
        //void remove_block(int index);
        void (*remove_block)(int) = (void(*)(int)) dlsym(handle, "remove_block");

        if (dlerror()) {
            fprintf(stderr, "Cannot load functions from library.so\n");
            exit(1);
        }
#endif

    if (argc <= 1) {
        printf("Too few args\n");
        exit(1);
    }

    char* cmd;
    for (int i = 1; i < argc; i++) {
        cmd = argv[i];

        if (strcmp(cmd, "create_table") == 0) {
            if (i + 1 >= argc) {
                printf("create_table expects one argument sadly\n");
                exit(4);
            }

            int size = atoi(argv[++i]);

            create_table(size);
        }

        else if (strcmp(cmd, "wc_files") == 0) {
            if (i + 1 >= argc) {
                printf("This excects at least 2 arguments happily\n");
                exit(1);
            }

            //tutaj trzeba znalezc ile argumentów wziąc i je przekazać do count...

            //count();

            int cnt = 0;
            int cpy = i+1;
            for (;cpy < argc; cpy++){
                if (strcmp(argv[cpy], "create_table") == 0 ||
                    strcmp(argv[cpy], "wc_files") == 0 ||
                    strcmp(argv[cpy], "remove_block") == 0 ||
                    strcmp(argv[cpy], "stimer") == 0 ||
                    strcmp(argv[cpy], "etimer") == 0){
                    break;
                }
                else {
                    cnt++;
                }
            }

            if (cnt == 0){
                fprintf(stderr, "Expected argument after wc_lines not next command\n");
                exit(4);
            }

            count(argv + i + 1, cnt);
            int next = read_table_from_file();
//            printf("     New block here: %d\n", next);
            i = cpy-1;
        }

        else if (strcmp(cmd, "remove_block") == 0) {
            if (i + 1 >= argc) {
                printf("remove_block expects one argument sadly\n");
                exit(4);
            }

            int idx = atoi(argv[++i]);

            remove_block(idx);
        }


#ifdef PERFORMANCE_TESTS
            else if (strcmp(cmd, "stimer") == 0) {
			start_timer();
		}
		else if (strcmp(cmd, "etimer") == 0) {
			stop_timer();
			print_times(argv[++i]);
		}
#endif
        else {
            printf("Bad argument mate: %s\n", cmd);
            break;
        }
    }

    free_table();

#ifdef LIB_DYNAMIC
    dlclose(handle);
#endif

#ifdef PERFORMANCE_TESTS
    printf("\n");
#endif

    return 0;
}
