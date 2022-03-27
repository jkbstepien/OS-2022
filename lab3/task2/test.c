#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/times.h>

clock_t start_time, end_time;
struct tms timer_start_tms, timer_end_tms;

void start_timer();
double calc_time(clock_t start, clock_t end);
void print_time();

double function(double x);
int RANGE_FROM = 0;
int RANGE_TO = 1;

int main(int argc, char** argv) {
    double rectangle_width = atof(argv[1]);

    start_timer();

    for (double i = RANGE_FROM; i < RANGE_TO;) {
        if (fork() == 0) {
            double result = function(i + rectangle_width/2) * rectangle_width;
            char filename[100];
            snprintf(filename, 100, "./partial_results/w%d.txt", getpid() - getppid());
            FILE* res_file = fopen(filename, "w+");
            fprintf(res_file, "%f", result);
            fclose(res_file);
            exit(0);
        }
        i += rectangle_width;
    }
    wait(0);

    double solution = 0;

    FILE* res_file;
    double tmp_res;
    char filename[100];
    char line[10];
    int i = 1;
    for (double j = RANGE_FROM; j < RANGE_TO;i++) {
        snprintf(filename, 100, "./partial_results/w%d.txt", i);
        res_file = fopen(filename, "r+");
        fgets(line, sizeof line, res_file);
        fclose(res_file);
        tmp_res = atof(line);
        solution += tmp_res;

        j += rectangle_width;
    }

    end_timer();
    print_time("calculation times:");

    printf("result: %f\n", solution);

    return 0;
}

double function(double x) {
    return 4 / (x * x + 1);
}

void start_timer() {
    start_time = times(&timer_start_tms);
}

void end_timer() {
    end_time = times(&timer_end_tms);
}

double calc_time(clock_t start, clock_t end) {
    return (double)(end - start)/ (double)sysconf(_SC_CLK_TCK);
}

void print_time(FILE* file) {
    fprintf(file,
        "real: %.4f user: %.4f sys: %.4f\n",
        calc_time(start_time, end_time),
        calc_time(timer_start_tms.tms_cutime, timer_end_tms.tms_cutime),
        calc_time(timer_start_tms.tms_cstime, timer_end_tms.tms_cstime));
}
