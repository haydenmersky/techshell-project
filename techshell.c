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

struct ShellCommand {
    char** args; // The arguments to the command, including the command itself as the first argument
    char* input_file; // The file to redirect input from, or NULL if no redirection
    char* output_file; // The file to redirect output to, or NULL if no redirection
};

// This is a function made so when the struct is made, and it can be easily freed up when necessary. Can't be used unless struct is complete.
void FreeStruct(struct ShellCommand* command) {
    if (command->args != NULL) {
        for (int i = 0; command->args[i] != NULL; i++) {
            free(command->args[i]);
        }
        free(command->args);
        command->args = NULL;
    }
    if (command->input_file != NULL) {
        free(command->input_file);
        command->input_file = NULL;
    }
    if (command->output_file != NULL) {
        free(command->output_file);
        command->output_file = NULL;
    }
}

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

// Parses the command prompt input into a ShellCommand structure. This function separates the command and its arguments, 
// as well as handling input and output redirection. It returns a ShellCommand struct with the parsed information to be used for ExecuteCommand().
struct ShellCommand ParseCommandLine(char* input) {
    struct ShellCommand command;

    // initializing everything to NULL for now
    command.args = NULL;
    command.input_file = NULL;
    command.output_file = NULL;

    // If the input is NULL or empty, returns an empty command struct
    if (input == NULL || strlen(input) == 0) { 
        return command;
    }

    int maxArgs = 64;
    int argCount = 0;
    // Allocate memory for arguments
    command.args = malloc(maxArgs * sizeof(char*)); 

    if (command.args == NULL) {
        perror("Error allocating memory for arguments.");
        return command;
    }

    char* inputCopy = strdup(input);
    if (inputCopy == NULL) {
        perror("Error duplicating input string.");
        // Frees the previously allocated memory for arguments. C doesn't clean up after itself, so we have to do it manually.
        free(command.args); 
        return command;
    }

    // Tokenize the input string by spaces
    char* token = strtok(inputCopy, " "); 

    // Loops through each token
    while (token != NULL) { 
        // Handles input redirection
        if (strcmp(token, "<") == 0) { 
            // Gets the next token which should be the input file name
            token = strtok(NULL, " "); 
            if (token != NULL) {
                // Uses strdup to allocate memory for the input file name
                command.input_file = strdup(token); 
            }
        }
        // Handles output redirection
        else if (strcmp(token, ">") == 0) { 
            token = strtok(NULL, " ");
            if (token != NULL) {
                // strdup to allocate memory for the output file name
                command.output_file = strdup(token); 
            }
        }
        else {
            if (argCount >= maxArgs - 1) { // can't just be argCount > maxArgs. not sure why, something to with the NULL part of the array
                // Doubles the size of the arguments array if we exceed the current limit
                maxArgs *= 2; 
                // Reallocates memory for arguments
                char** newArgs = realloc(command.args, maxArgs * sizeof(char*)); 
                if (newArgs == NULL) {
                    perror("Error reallocating memory for arguments.");
                    for (int i = 0; i < argCount; i++) {
                        free(command.args[i]);
                    }
                    free(command.args);
                    free(inputCopy);
                    if (command.input_file) free(command.input_file);
                    if (command.output_file) free(command.output_file);
                    command.args = NULL;
                    command.input_file = NULL;
                    command.output_file = NULL;
                    return command;
                }
                command.args = newArgs;
            }
            command.args[argCount] = strdup(token);
            argCount++;
        }
        token = strtok(NULL, " ");
    }
    command.args[argCount] = NULL;
    free(inputCopy);
    return command;
}

//void ExecuteCommand(struct ShellCommand command); // Execute a shell command

int main() {
    char* input;
    struct ShellCommand command;

    // repeatedly prompt the user for input
    for (;;)
    {
        input = CommandPrompt();
        // If there was an error getting user input, break. Something is likely wrong
        if (input == NULL) { 
            printf("\n");
            break;
        }

        // parse the command line
        command = ParseCommandLine(input);
        // If the command is empty, just continue to the next iteration of the loop
        if (command.args == NULL) { 
            free(input);
            continue;
        }

        // for Parse testing purposes, print the parsed command and arguments. Ignore in final product.
        printf("Command: %s\n", command.args[0]);
        printf("Arguments:\n");
        for (int i = 0; command.args[i] != NULL; i++) {
            printf("  %s\n", command.args[i]);
        }

        // execute the command
        //ExecuteCommand(command);

        // Frees the memory allocated for the command and its arguments
        FreeStruct(&command); 
        // Frees the memory allocated by getcwd
        free(input); 
    }
    exit(0);

}
