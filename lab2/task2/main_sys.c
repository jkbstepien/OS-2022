#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/times.h>

clock_t start_time, end_time;
struct tms timer_start_tms, timer_end_tms;

void start_timer();
void end_timer();
double calc_time(clock_t start, clock_t end);
void print_time(FILE* file, int count_c, int count_lines);

int count_characters(char* character, char* file);
int count_linecharacters(char* character, char* file);

int main(int argc, char** argv){
    char* character_normal;
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

            character_normal = calloc(1, sizeof(char));
            buffer = calloc(256, sizeof(char));
            scanf("%s1", character_normal);
            scanf("%s256", buffer);
        }
        else {
            character_normal = argv[1];
            buffer = argv[2];
        }
    }

    start_timer();

    FILE* result_file = fopen("pomiar_zad_2.txt", "w");

    int counter1 = count_characters(character_normal, buffer);
    int counter2 = count_linecharacters(character_normal, buffer);

    end_timer();
    print_time(result_file, counter1, counter2);

    fclose(result_file);

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

int count_characters(char* character, char* file) {
    int input_file1 = open(file, O_RDONLY);
    
    int count_chars = 0;
    char ch;

    if (input_file1 == -1) {
        return -1;
    }
    
    do {
        if ((int) ch == *character) {
            count_chars++;
        }

    } while(read(input_file1, &ch, sizeof(char)));

    close(input_file1);

    return count_chars;
}


int count_linecharacters(char* character, char* file){
    int input_file2 = open(file, O_RDONLY);

    int curr_line = 0;
    int count_char_in_line = 0;
    char ch;

    if (input_file2 == -1) {
        return -1;
    }

    do {
        if ((int) ch == *character) {
            if (count_char_in_line == 0) {
                count_char_in_line++;
            }
        }
        if((int) ch == '\n') {
            if(count_char_in_line != 0) {
                curr_line++;
                count_char_in_line = 0;
            }
        }
    } while (read(input_file2, &ch, sizeof(char)));

    close(input_file2);

    return curr_line;
}