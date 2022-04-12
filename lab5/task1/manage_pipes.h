//
// Created by jkbstepien on 12.04.2022.
//

#ifndef LAB5_MANAGE_PIPES_H
#define LAB5_MANAGE_PIPES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include "parse.h"

void manage_pipes(char **commands, int no_lines, int p_input[2], int p_output[2], int i, int j, int k) {
    if (fork() == 0) {
        if (j == 0 && i == 0) {
            close(p_output[0]);
            dup2(p_output[1], STDOUT_FILENO);
        }
        else if (j == k - 1 && i == no_lines - 1) {
            close(p_input[1]);
            close(p_output[0]);
            close(p_output[1]);
            dup2(p_input[0], STDIN_FILENO);
        }
        else {
            close(p_input[1]);
            close(p_output[0]);
            dup2(p_input[0], STDIN_FILENO);
            dup2(p_output[1], STDOUT_FILENO);
        }

        char file_path[1000];
        char** args = parse_sep_prog_name_args(commands[j]);
        strcpy(file_path, args[0]);
        execvp(file_path, args);
    } else {
        close(p_input[1]);
        p_input[0] = p_output[0];
        p_input[1] = p_output[1];
        pipe(p_output);
    }
}

int IDX = 0;
char *ARRAY[100];

int get_line_number(char *line) {
    for (int i = 0; i < IDX; ++i) {
        if (strcmp(line, ARRAY[i]) == 0) {
            return i;
        }
    }
    return -1;
}

void get_output(FILE *file) {

    // Initialize buffers to store file contents.
    char **buffer_all_lines = (char **) calloc(1000, sizeof(char *));
    char *buffer_one_line = (char *) calloc(1000, sizeof(char));

    // Read file line by line.
    int line_current_number = 0;
    while (fgets(buffer_one_line, 1000 * sizeof(char), file)) {
        printf("\nLine contents: %s", buffer_one_line);
        int *no_lines = prepare_lines_to_exec(buffer_one_line);
        int lines_number = 0;
        int p_input[2];
        int p_output[2];
        pipe(p_output);

        if (strstr(buffer_one_line, "=")) {
            // Processing declared elements.
            char *line_copy = (char *) calloc(1000, sizeof(char));
            strcpy(line_copy, buffer_one_line);
            buffer_all_lines[line_current_number] = line_copy;

            char *line_copy2 = (char *) calloc(1000, sizeof(char));
            strcpy(line_copy2, buffer_one_line);

            char *word = strtok(line_copy2, " ");
            ARRAY[IDX++] = word;
            line_current_number++;
        } else {
            // Executing declared elements.
            while (no_lines[lines_number] != -1) {
                lines_number++;
            }

            for (int i = 0; i < lines_number; i++) {
                char **commands = parse_sep_by_eq(buffer_all_lines[no_lines[i]]);

                int ccounter = -1;
                while (commands[++ccounter] != NULL) {
                    printf("\tFound command: %s\n", commands[ccounter]);
                }

                for (int j = 0; j < ccounter; j++) {
                    manage_pipes(commands, lines_number, p_input, p_output, i, j, ccounter);
                }
            }
        }
        while ((wait(0)) > 0);
    }
}
#endif //LAB5_MANAGE_PIPES_H
