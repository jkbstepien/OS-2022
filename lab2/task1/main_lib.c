#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sys/times.h"
#include "unistd.h"

clock_t start_time, end_time;
struct tms timer_start_tms, timer_end_tms;

void start_timer();
double calc_time(clock_t start, clock_t end);
void print_time();

int main(int argc, char** argv) {
    // buffer_in - input from file1.
    // buffer_out - value we want to copy to file2.
    char* buffer_in = calloc(256, 1);
    char* buffer_out = calloc(256, 1);

    // Validate number of program arguments.
    if (argc > 3 || argc == 2) {
        printf("Incorrect number of arguments. There are too many!");
    }
    else {
        if (argc < 3) {
            // If arguments are lacking - prompt user to fill them.
            printf("Files you want to process:\n");
            scanf("%start", buffer_in);
            scanf("%start", buffer_out);
        }
        else {
            strcpy(buffer_in, argv[1]);
            strcpy(buffer_out, argv[2]);
        }
    }

        // Copy contents of file1 to file2 and measure time.
        start_timer();

        FILE* input_file = fopen(buffer_in, "r");
        FILE* output_file = fopen(buffer_out, "w");
        FILE* time_file = fopen("pomiar_zad_1.txt", "w");

        if (input_file == NULL) {
            printf("Error while opening the file!");
            return -1;
        }

        char character;
        int i;
        do {
            fread(&character, sizeof(char), 1, input_file);
            if (character != '\n'){
                i = 0;
                fwrite(&character, sizeof(char), 1, output_file);
            } else {
                i++;
                if (i == 1) {
                    fwrite(&character, sizeof(char), 1, output_file);
                }
            }
        } while (feof(input_file) == 0);

        fclose(input_file);
        fclose(output_file);

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