#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/times.h>

clock_t start_time, end_time;
struct tms timer_start_tms, timer_end_tms;

void start_timer();
void end_timer();
double calc_time(clock_t start, clock_t end);
void print_time();

int main(int argc, char **argv) {

  if (argc != 3) {
    printf("Incorrect number of arguments! Expected 2.\n");
    exit(0);
  }

  start_timer();

  /** Store:
     * interval - rectangle width.
     * total_p - processes to handle.
     * part_result - result obtained from individual file wx.txt
     * result - total area from calculations.
     **/
  double interval = atof(argv[1]);
  int total_p = atoi(argv[2]);
  double part_result = 0.0;
  double result = 0.0;

  for (int i = 0; i < total_p; i++) {
    if (fork() == 0) {
      part_result = 0.0;

      for (double x = (double)i / total_p; x < (double)(i + 1) / total_p; x += interval) {
        part_result += (4 * interval) / (x * x + 1);
      }

      char filename[100];
      snprintf(filename, 100, "./results/w%d.txt", i);
      
      FILE *file = fopen(filename, "w");
      fprintf(file, "%f", part_result);
      fclose(file);

      return 0;
    }
  }

  for (int i = 0; i < total_p; i++) {
    wait(0);
  }

  part_result = 0.0;
  char filename[100];

  for (int i = 0; i < total_p; i++) {
    snprintf(filename, 100, "./results/w%d.txt", i);
    FILE *file = fopen(filename, "r");
    fscanf(file, "%lf", &part_result);
    fclose(file);

    result += part_result;
  }

  printf("%f\n", result);

  FILE* time_file = fopen("report.txt", "w");
  end_timer();
  print_time(time_file);
  fclose(time_file);

  return 0;
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