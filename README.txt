Names: Hayden Mersky, Nicholas Sanders, Ashish Ghimire

Program that allows a user to enter commands to be interpreted by a shell.
The program utilizes 3 methods in conjunction with one another: CommandPrompt(), 
ParseCommandLine(), and ExecuteCommand(). CommandPrompt() receives the command,
which is then parsed by ParseCommandLine() and sent to ExecuteCommand() to be
carried out in a child process utilizing execvp
The shell also supports input and output redirection using < and >.
The shell continues to prompt the user until "exit" is entered and the program terminates.

CommandPrompt()
    CommandPrompt() displays a shell style environment for commands to be entered
into, then routes those commands to be parsed. It first uses getcwd() to find the
current working directory, and dynamically allocate memory to store the directory.
Error handling is also implemented in case of a NULL directory. Next, the working
directory is displayed in a way similar to a normal shell, and getline() is used 
to take in user input. Since getline() dynamically allocates memory automatically, 
commands of any length can be read. The input string is cleaned up by removing the 
included newline, and any errors are handled before the command is sent to ParseCommandLine().

ParseCommandLine()
    ParseCommandLine() takes the input given from CommandPrompt() and essentially
divides it into the initial command, the arguments potentially given, as well as
the possible input or output direction. It does this by putting the input through 
a defined ShellCommand structure. It begins by initializing one with every field 
set to NULL. The memory is dynamically allocated for the arguments array, starting 
at a capacity of 64 slots and doubling via realloc() if that limit is exceeded. The 
input string is duplicated with strdup() to preserve the original, then tokenized 
by spaces using strtok(). As each token is processed, the function checks whether 
it is a redirection operator ('<' or '>'). If found, the following token is stored 
as an input or output file using strdup(). All other tokens are treated as command
arguments and appended to the args array. THe args array is NULL-terminated at the
end, the duplication input string is freed, and the completed ShellCommand struct
is returned to main() to be passed into ExecuteCommand().

ExecuteCommand()