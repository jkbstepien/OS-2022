#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mylib.h"

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
    printf(
            "%20s real %.3fs user %.3fs sys %.3fs\n",
            operation,
            calc_time(clock_t_begin, clock_t_end),
            calc_time(times_start_buffer.tms_cutime, times_end_buffer.tms_cutime),
            calc_time(times_start_buffer.tms_cstime, times_end_buffer.tms_cstime)
            );
}
#endif

int main(int argc, char** argv) {
    if (argc <= 1) {
        printf("Too few args\n");
        exit(1);
    }

}
