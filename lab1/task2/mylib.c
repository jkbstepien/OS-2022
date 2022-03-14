#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mylib.h"

char system_command[10000000];
char** memory = NULL;
int memory_size = 0;
int memory_free_index = 0;

void create_table(int size) {
	if (size <= 0) {
        fprintf(stderr, "Size can not be a negative number!");
        return;
    }
	if (memory != NULL) {
        fprintf(stderr, "Memory already allocated!");
        return;
    }

	memory = calloc(size, sizeof(char*));
	memory_size = size;
}

//void free_table() {
//	for (int i = 0; i < memory_size; i++) {
//		if (memory[i] != NULL) {
//			free(memory[i]);
//			memory[i] = NULL;
//		}
//	}
//
//	free(memory);
//}

void wc_files(char* file_desc) {
//    char* result_string;
    system_command[0] = '\0';
    strcat(system_command, file_desc);
    strcat(system_command, " > tmp");
    system(system_command);
//
//    strcat(system_command, "wc ");
//    for (int i = 0; i < size; i++) {
//        result_string = strcat(file_desc[i], " ");
//        strcat(system_command, result_string);
//    }
//    printf("%s", result_string);
//    strcat(system_command, ">> tmp");
//
//    system(system_command);
}

long get_file_size(FILE* file) {
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    return size;
}

//int get_free_block_index() {
//    for (int i = 0; i < memory_size; i++) {
//        if (memory[i] != NULL) {
//            return i;
//        }
//    }
//
//    fprintf(stderr, "There are no available free blocks!");
//    return -1;
//}

int load_block_to_table() {
    FILE* file;
    int saved_at_index = 0;

    file = fopen("tmp", "rb");

    long size = get_file_size(file);
    char* wrapper = calloc(size, sizeof(char));
//    memory[index] = calloc(size, sizeof(char));
//    fscanf(filep, "%s", memory[index]);
    fread(wrapper, sizeof(char), size, file);

    fclose(file);

    memory[memory_free_index] = wrapper;
    saved_at_index = memory_free_index;
    memory_free_index++;

    return saved_at_index;
}

void remove_block(int index) {
    if (index < 0 || index >= memory_size) {
        fprintf(stderr, "Index out of range!");
        return;
    }
    if (memory_free_index == 0) {
        fprintf(stderr, "Empty table - you can not remove data!");
        return;
    }
    if (memory[index] == NULL) {
        fprintf(stderr, "This block does not have data!");
        return;
    }

    free(&memory[index]);
    free(memory[index]);
    memory[index] = memory[memory_free_index - 1];
    memory_free_index--;
}
