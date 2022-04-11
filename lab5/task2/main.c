#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <resolv.h>

int main(int argc, char **argv) {

    // Validate number of program arguments.
    if (argc != 2 && argc != 4) {
        printf("Incorrect number of arguments. Expected 2 or 4.");
        exit(1);
    } else if (argc == 2) {
        // Create command variable for popen to execute.
        char *command;

        // mail -H returns header summary, then we sort our values by 3rd key.
        if (strcmp(argv[1], "sender") == 0) {
            command = "echo | mail -H | sort -k 3";
        } else if (strcmp(argv[1], "date") == 0) {
            command = "echo | mail -H";
        } else {
            printf("Incorrect arguments. Closing program..");
            exit(1);
        }

        // Print sorted emails by date or sender.
        FILE *output_file = popen(command, "r");
        char curr_position_line[512];
        while (fgets(curr_position_line, 512, output_file) != NULL) {
            printf("%s\n", curr_position_line);
        }

        return 0;
    } else {
        // Send email with given subject - "s" flag.
        char command[512];
        snprintf(command, sizeof(command), "echo %s | mail %s -s %s", argv[3], argv[1], argv[2]);
//         Code below is commented because I live in student dorm, and it is restricted to send mails from port 25.
//        popen(command, "r");
    }

    return 0;
}