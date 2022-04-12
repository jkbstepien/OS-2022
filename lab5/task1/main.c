//
// Created by jkbstepien on 12.04.2022.
//
#include <stdio.h>
#include <stdlib.h>
#include "manage_pipes.h"

int main(int argc, char **argv) {

    if (argc != 2) {
        printf("Incorrect number of arguments. Expected 2.");
        exit(EXIT_FAILURE);
    }

    char *path = argv[1];
    FILE *file = fopen(path, "r");
    get_output(file);
    fclose(file);

    return 0;
}
