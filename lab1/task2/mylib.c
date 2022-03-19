#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mylib.h"

char system_command[10000000] = "wc ";
char** memory = NULL;
int memory_size = 0;
int memory_working_size = 0;
int memory_free_index = 0;

void create_table(int size) {
    if (size <= 0) {
        fprintf(stderr, "[create table] Size can not be a negative number!\n");
        return;
    }
    if (memory != NULL) {
        fprintf(stderr, "[create_table] Memory already allocated!\n");
        return;
    }

    memory = calloc(size, sizeof(char*));
    memory_size = size;
}

void wc_files(char* file_desc) {

    strcat(system_command, file_desc);
    strcat(system_command, " > tmp");

    system(system_command);
}

long get_file_size(FILE* file) {
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    return size;
}

int load_block_to_table() {
    FILE* file;
    int saved_position;

    file = fopen("tmp", "r");

    long size = get_file_size(file);
    char* wrapper = calloc(size, sizeof(char));
    fread(wrapper, sizeof(char), size, file);

    fclose(file);

    memory[memory_free_index] = wrapper;
    saved_position = memory_free_index;
    memory_free_index++;

    return saved_position;
}

void remove_block(int index) {

    if (index < 0 || index >= memory_size) {
        fprintf(stderr, "[remove block] Index out of range!\n");
        return;
    }
    if (memory_free_index == 0) {
        fprintf(stderr, "[remove block] Empty table - you can not remove block!\n");
        return;
    }
    if (memory[index] == NULL) {
        fprintf(stderr, "[remove block] This block does not have data!\n");
        return;
    }
    if (index == memory_free_index - 1) {
        free(memory[index]);
        memory_free_index--;
        return;
    }

    free(&memory[index]);
    free(memory[index]);
    memory[index] = memory[memory_free_index - 1];
    memory_free_index--;
}
