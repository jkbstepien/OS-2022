#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

void dig_in(char* dirpath, char* pattern, int depth, int current_depth);

int main(int argc, char** argv) {
    
    if (argc != 4) {
        printf("Incorrect number of arguments! Expected 3 arguments.");
        exit(1);
    }
    if (atoi(argv[3]) < 0) {
        printf("Recursion depth can not have negative value!");
        exit(1);
    }

    char* dir_begin = argv[1];
    char* pattern = argv[2];
    int depth = atoi(argv[3]);
    int current_depth = 0;

    if (fork() == 0) {
        dig_in(dir_begin, pattern, depth, current_depth);
        exit(0);
    }

    return 0;
}

void dig_in(char* dirpath, char* pattern, int depth, int current_depth) {
    
    char current_path[1000];
    DIR* dir;
    struct dirent* dir_file;

    if ((dir = opendir(dirpath)) != NULL && current_depth > depth) {
        return;
    }

    while ((dir_file = readdir(dir)) != NULL) {

        // Omit hidden dirs.
        if (strcmp(dir_file -> d_name, ".") == 0 || strcmp(dir_file -> d_name, "..") == 0) {
            continue;
        }

        // Create actual path on the go.
        strcpy(current_path, dirpath);
        strcat(current_path, "/");
        strcat(current_path, dir_file -> d_name);

        // Check if our file is a text file.
        if (strstr(dir_file -> d_name, ".txt")) {
            FILE* file = fopen(current_path, "r");
            
            fseek(file, 0, SEEK_END);
            long file_size = ftell(file);
            rewind(file);
            char* buffer = calloc(file_size, sizeof(char));

            fread(buffer, sizeof(char), file_size, file);

            if (strstr(buffer, pattern)){
                    printf("found pattern in: %s \nprocess id: %d\n", current_path, (int) getpid());
                }
            
            free(buffer);
            fclose(file);
        }

        // In other case - create new process and go deeper with recursion.
        if (dir_file -> d_type == DT_DIR) {
            if (fork() == 0) {
                dig_in(current_path, pattern, depth, current_depth + 1);
                exit(0);
            }
        }
    }
    closedir(dir);
}