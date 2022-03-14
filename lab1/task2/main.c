#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "mylib.h"

int main(int argc, char** argv) {
    // Program requires at least two arguments.
    assert(argc > 1);

    for (int i = 0; i < argc; i++) {

        if (strcmp(argv[i], "create_table") == 0) {
            // create_table has mandatory argument "size".
            assert(i + 1 < argc);

            create_table(atoi(argv[++i]));
        }
        else if (strcmp(argv[i], "wc_files") == 0) {
            // wc_files require at least one program argument.
            assert(i + 1 < argc);

            wc_files(argv[i++]);

//            // Count program arguments from input.
//            int count_args = 0;
//            int index = i + 1;
//            for (; index < argc; index++) {
//                if (strcmp(argv[index], "create_table") == 0 ||
//                        strcmp(argv[index], "wc_files") == 0 ||
//                        strcmp(argv[index], "remove_block") == 0) {
//                    break;
//                }
//                else {
//                    count_args++;
//                }
//            }
//            // To call wc_files it is needed to pass at least one argument.
//            assert(count_args > 0);
//
//            wc_files(argv + i + 1, count_args);
//            i = index - 1;

        }
        else if (strcmp(argv[i], "remove_block") == 0) {
            // remove_block require one argument "index".
            assert(i + 1 < argc);

            remove_block(atoi(argv[++i]));
        }
    }

    return 0;
}
