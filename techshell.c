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

//Functions to implement:
char* CommandPrompt(){ // Display current working directory and return user input
    char path[PATH_MAX]; // define buffer for getcwd command

    // Error handling for getcwd
    if (getcwd(path, sizeof(path)) != NULL) { // If successful, convert to char* and return
        char* cwd = getcwd(path, sizeof(path));
        return cwd;
    } else { // Otherwise, print error and return NULL
        perror("getcwd() error");
        return NULL;
    }

}
struct ShellCommand ParseCommandLine(char* input); // Process the user input (As a shell command)

void ExecuteCommand(struct ShellCommand command); //Execute a shell command

int main() {
    char* input;
    struct ShellCommand command;

    // repeatedly prompt the user for input
    for (;;)
    {
        input = CommandPrompt();
        // parse the command line
        command = ParseCommandLine(input);
        // execute the command
        ExecuteCommand(command);
    }
    exit(0);

}
