#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "mylib.h"

#define MEMORY_TYPE char*

char system_command[10000000];
char* memory = NULL;
int memory_size = 0;

void create_table(int size) {
	if (size <= 0) {
        fprintf(stderr, "Size can not be a negative number!");
        return;
    }
	if (memory != NULL) {
        fprintf(stderr, "Memory already allocated!");
        return;
    }

	memory = calloc(size, sizeof(MEMORY_TYPE));
	memory_size = size;
}

void free_table() {
	for (int i = 0; i < memory_size; i++) {
		if (memory[i] != NULL) {
			free(memory[i]);
			memory[i] = NULL;
		}
	}

	free(memory);
}

void count_elements(char** file_desc, int size) {
    assert(size > 0);

    char* result_string;
    system_command[0] = '\0';

    strcat(system_command, "wc ");
    for (int i = 0; i < size; i++) {
        result_string = strcat(file_desc[i], " ");
        strcat(system_command, result_string);
    }
    strcat(system_command, "> tmp");

    system(system_command);
}

long get_file_size(FILE* fd) {
    fseek(fd, 0, SEEK_END);
    long size = ftell(fd);
    fseek(fd, 0, SEEK_SET);
    return size;
}

int get_empty_block_index() {
    for (int i = 0; i < memory_size; i++) {
        if (memory[i] != NULL) {
            return i;
        }
    }

    fprintf(stderr, "There are no available free blocks!");
    return -1;
}

int load_block() {
    FILE* filep;

    filep = fopen("tmp", "r");

    int index = get_empty_block_index();
    long size = get_file_size(filep);

    memory[index] = calloc(size, sizeof(char));
    fscanf(filep, "%s", memory[index]);
    *(memory + size) = '\0';

    fclose(filep);

    return index;
}

void free_block(int index) {
    if (index >= memory_size) {
        fprintf(stderr, "Index out of range!");
        return;
    }
    if (memory[index] == NULL) {
        fprintf(stderr, "This block does not have data!");
        return;
    }

    free(memory[index]);
    memory[index] = NULL;
    memory_size--;
}
