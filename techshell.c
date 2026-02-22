/*
* Name(s): Hayden Mersky, Nicholas Sanders, Ashish Ghimire
* Date: 02/09/2026
* Description: Program that allows a user to enter commands to be interpreted by a shell.
The program utilizes 3 methods in conjunction with one another: CommandPrompt(), 
ParseCommandLine(), and ExecuteCommand(). CommandPrompt() receives the command,
which is then parsed by ParseCommandLine() and sent to ExecuteCommand() to be
carried out in a child process utilizing execvp. 
The shell also supports input and output redirection using < and >.
The shell continues to prompt the user until "exit" is entered and the program terminates.
*
*
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <errno.h>

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
// Function excutes a parsed sheel commmand 
// Returns 1 if the shell should termintat else 0 
int ExecuteCommand(struct ShellCommand command){

    // if there is no command do nothing
    if (command.args == NULL || command.args[0] == NULL) {
        return 0;
    }
    
    // "exit" smust terminate the shell
    if (strcmp(command.args[0], "exit") == 0) {
        return 1; // telling main to stop
    }

    // cd must run in parent process , if ran in the child , directory would disappear
    if (strcmp(command.args[0], "cd") == 0) {
        // first args should contain the path after cd 
        char* target = command.args[1];
    

    // if user typed only cd go to home directory
        if (target == NULL){
        target = getenv("HOME");
     }
    
    // change thie directory 
    if (chdir(target) != 0) {
        // printing error message if cd fails
        fprintf(stderr ,"cd: %s: %s\n", target, strerror(errno));
    }

    return 0;
}

// forking duplicate the current process 
// parent and child continue from here 

pid_t pid = fork();

if (pid < 0){
    fprintf(stderr, "Error %d (%s)\n", errno, strerror(errno));
    return 0;
}

// child process
if (pid == 0){
    // input redirection < if  parser detected an input file 
    if (command.input_file != NULL){

        // open the file for reading 
        FILE* infile = fopen(command.input_file, "r");
        
        // if file counldnt open
        if (infile == NULL){
            fprintf(stderr, "Error %d (%s)\n", errno, strerror(errno));
            _exit(1); // terminate child 
        }

        // replace stdin fd(0) with open file 
        // fileno() converting file* to fd
        if (dup2(fileno(infile), STDIN_FILENO) < 0){
            fprintf(stderr, "Error %d (%s)\n", errno, strerror(errno));
            fclose(infile);
            _exit(1);
        }
        // close original file* stdin now points to the same file 
        fclose(infile);
    }
    // output redirectioon >

    if(command.output_file != NULL){
        // open file 
        FILE* outfile = fopen(command.output_file, "w");

        if (outfile == NULL){
            fprintf(stderr, "Error %d (%s)\n", errno, strerror(errno));
            _exit(1);
        }

        //replace stdout fd(1) with file 
        if(dup2(fileno(outfile), STDOUT_FILENO) < 0){
            fprintf(stderr, "Error %d (%s)\n" ,errno, strerror(errno));
            fclose(outfile);
            _exit(1);
        }
        fclose(outfile);
    }

    //execute program
    // execvp replaces this child process ,with the request program
    execvp(command.args[0], command.args);

    // if execvp returns there is error 
    fprintf(stderr, "Error %d (%s)\n", errno, strerror(errno));

    _exit(1); // end child process
}

    // parent process 
    //parents waits for child to finish preventing prompt apearing early 
    int status;
    waitpid(pid, &status,0);

    return 0;


}

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
        // printf("Command: %s\n", command.args[0]);
        // printf("Arguments:\n");
        // for (int i = 0; command.args[i] != NULL; i++) {
        //     printf("  %s\n", command.args[i]);
        // }

        // execute the command
        //ExecuteCommand(command);
        int shouldExit = ExecuteCommand(command);
        // Frees the memory allocated for the command and its arguments
        FreeStruct(&command); 
        // Frees the memory allocated by getcwd
        free(input); 
        
        if (shouldExit) {
            break;
        }
    }
    exit(0);

}
