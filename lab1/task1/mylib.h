#ifndef TASK1_MYLIB_H
#define TASK1_MYLIB_H

void create_table(int size);
void free_table();

void count_elements();
long get_file_size(FILE* fd);
int get_empty_block_index();

void create_block();
int load_block();
void free_block();

#endif //TASK1_MYLIB_H
