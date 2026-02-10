/*
* Name(s): Hayden Mersky, Nicholas Sanders, Ashish Ghimire
* Date: 02/09/2026
* Description: **Include what you were and were not able to handle!**
*
*
*/
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

//Functions to implement:
char* CommandPrompt(){ // Display current working directory and return user input
    char* cwd = getcwd(NULL, 0); // get current working directory, also mallocs automatically

    if (cwd == NULL) { // Error handling
        perror("getcwd() error");
        return NULL;
    }

    // Prompt the user for input
    printf("%s$ ", cwd);
    free(cwd); // Free the memory allocated by getcwd to prevent memory leaks

    // Read the input
    char* input = NULL;
    size_t size = 0; // C's unsigned integer type, needed because of potential size issues with int

    if (getline(&input, &size, stdin) == -1) { // Read user input from getline. Use getline to prevent issues from an unknown input size
        perror("getline() error");
        free(input);
        return NULL;
    }

    // Remove the newLine character, quirk of getline
    input[strcspn(input, "\n")] = '\0'; // strcspn returns index of first "\n", then we set that index to '\0' to remove

    return input;
}

int main() {
    char* input;
    //struct ShellCommand command;

    // repeatedly prompt the user for input
    for (;;)
    {
        input = CommandPrompt();
        // parse the command line
        //command = ParseCommandLine(input);
        // execute the command
        //ExecuteCommand(command);
    }
    exit(0);

}
