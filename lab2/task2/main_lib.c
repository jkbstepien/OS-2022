#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/times.h>


clock_t start_time, end_time;
struct tms timer_start_tms, timer_end_tms;

void start_timer();
void end_timer();
double calc_time(clock_t start, clock_t end);
void print_time(FILE* file, int count_c, int count_lines);

int count_characters(char chosen_character, char* buffer){
    FILE* input_file = fopen(buffer, "r");

    if (input_file == NULL) {
        return -1;
    }

    char chosen_character;
    int signCounter = 0;
    do {
        fread(&chosen_character, sizeof(char), 1, input_file);
        if (chosen_character == chosen_character){
            signCounter++;
        }

    } while (feof(input_file) == 0);
    fclose(input_file);

    return signCounter;
}


int count_characterlines(char chosen_character, char* buffer){
    FILE* input_file = fopen(buffer, "r");

    if (input_file == NULL){
        return -1;
    }

    char chosen_character;
    int ctr_line = 0;
    int newLineSignCounter = 0;
    do {
        fread(&chosen_character, sizeof(char), 1, input_file);
        if ((int) chosen_character == '\n') {
            if(newLineSignCounter != 0) {
                ctr_line++;
                newLineSignCounter = 0;
            }
        }
        if (chosen_character == chosen_character) {
            if (newLineSignCounter == 0) {
                newLineSignCounter++;
            }
        }

    } while (feof(input_file) == 0);
    fclose(input_file);

    return ctr_line;
}

int main(int argc, char** argv){
    // chosen_character - chosen_character we search for in a file.
    // buffer - input from file.
    char* character;
    char* buffer;

    // Validate number of program arguments.
    if (argc > 3) {
        printf("Incorrect number of arguments. Expected 2 arguments.");
        return -1;
    }
    else {
        if (argc == 1 || argc == 2) {
            // If arguments are lacking - prompt user to fill them.
            printf("Character to search for and source file:\n");

            character = calloc(256, sizeof(char));
            buffer = calloc(256, sizeof(char));

            scanf("%s", character);
            scanf("%s", buffer);
        }
        else {
            character = argv[1][0];
            buffer = argv[2];
        }
    }

    // Count occurences of a char and number of lines with this char.
    start_timer();

    FILE* time_file = fopen("pomiar_zad_2.txt", "w");

    int count_c = count_characters(character, buffer);
    int count_clines = count_characterlines(character, buffer);

    end_timer();
    print_time(time_file, count_c, count_clines);

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

void print_time(FILE* file, int count_c, int count_clines) {
    fprintf(file, "Searching this file returned following results:\n\n");
    fprintf(file, "\tThere are %d characters in total %d lines.\n",
        count_c,
        count_clines);
    fprintf(file, "\tMeasured times:\n\t\treal: %.4f user: %.4f sys: %.4f\n",
        calc_time(start_time, end_time),
        calc_time(timer_start_tms.tms_cutime, timer_end_tms.tms_cutime),
        calc_time(timer_start_tms.tms_cstime, timer_end_tms.tms_cstime));
}
