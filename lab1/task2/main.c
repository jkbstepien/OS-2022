#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>

#include "mylib.h"

/* Time measurement */
struct tms start_cpu, end_cpu;
clock_t clock_start, clock_end;
void start_timer();
void end_timer();
void print_time(char* command, FILE* file);

int main(int argc, char** argv) {

    if (argc < 1) {
        fprintf(stderr, "[main] Too few arguments passed!");
        return -1;
    }

    // char* file_desc[] = "report2.txt";
    FILE* result_file = fopen("report2.txt", "a");

    for (int i = 0; i < argc; i++) {

        if (strcmp(argv[i], "start_timer") == 0) {
            start_timer();
        }
        else if (strcmp(argv[i], "end_timer") == 0) {
            end_timer();
        }
        else if (strcmp(argv[i], "create_table") == 0) {
            // create_table has mandatory argument "size".
            assert(i + 1 < argc);

            start_timer();

            create_table(atoi(argv[++i]));

            end_timer();
            print_time(argv[i - 1], result_file);
        }
        else if (strcmp(argv[i], "wc_files") == 0) {
            // wc_files require at least one program argument.
            assert(i + 1 < argc);
            
            start_timer();

            wc_files(argv[++i]);

            end_timer();
            print_time(argv[i - 1], result_file);
        }
        else if (strcmp(argv[i], "load_block_to_table") == 0) {
            start_timer();

            load_block_to_table();

            end_timer();
            print_time(argv[i], result_file);
        }
        else if (strcmp(argv[i], "remove_block") == 0) {
            // remove_block require one argument "index".
            assert(i + 1 < argc);

            start_timer();

            remove_block(atoi(argv[++i]));

            end_timer();
            print_time(argv[i - 1], result_file);
        }
    }

    fclose(result_file);

    return 0;
}

void start_timer() {
    clock_start = times(&start_cpu);
}

void end_timer() {
    clock_end = times(&end_cpu);
}

void print_time(char* command, FILE* file) {
    printf("%s \n\treal %.4fs sys %.4fs user %.4fs\n",
            command,
           ((double) clock_end - (double) clock_start) / (double) sysconf(_SC_CLK_TCK),
           ((double) end_cpu.tms_cstime - (double) start_cpu.tms_cstime) / (double) sysconf(_SC_CLK_TCK),
           ((double) end_cpu.tms_cutime - (double) start_cpu.tms_cutime) / (double) sysconf(_SC_CLK_TCK));
    fprintf(file, "%s \n\treal %.4fs sys %.4fs user %.4fs\n",
            command,
           ((double) clock_end - (double) clock_start) / (double) sysconf(_SC_CLK_TCK),
           ((double) end_cpu.tms_cstime - (double) start_cpu.tms_cstime) / (double) sysconf(_SC_CLK_TCK),
           ((double) end_cpu.tms_cutime - (double) start_cpu.tms_cutime) / (double) sysconf(_SC_CLK_TCK)
    );
}