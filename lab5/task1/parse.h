//
// Created by jkbstepien on 12.04.2022.
//

#ifndef LAB5_PARSE_H
#define LAB5_PARSE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include "manage_pipes.h"

char **parse_sep_by_eq(char *line) {
    char **commands = (char **) calloc(1000, sizeof(char *));
    char *span;

    strtok(line, "=");

    int i = 0;
    while ((span = strtok(NULL, "|")) != NULL) {
        commands[i++] = span;
    }

    return commands;
}

char **parse_sep_prog_name_args(char *command) {
    char **arguments = (char **) calloc(1000, sizeof(char *));
    char *elem = strtok(command, " ");
    int idx = 0;

    arguments[idx++] = elem;
    while ((elem = strtok(NULL, " ")) != NULL) {
        arguments[idx++] = elem;
    }
    arguments[idx] = NULL;

    return arguments;
}

int *prepare_lines_to_exec(char *line) {
    char **lines = (char **) calloc(100, sizeof(char *));
    char *elem = strtok(line, "|");
    int idx = 0;
    static int no_lines[100];

    lines[idx++] = elem;
    elem[strlen(elem) - 1] = '\0';

    while ((elem = strtok(NULL, "|")) != NULL) {
        lines[idx++] = elem;
    }

    int i = 0;
    while (lines[i] != NULL) {
        no_lines[i] = get_line_number(lines[i]);
        i++;
    }
    no_lines[i] = -1;

    return no_lines;
}

#endif //LAB5_PARSE_H
