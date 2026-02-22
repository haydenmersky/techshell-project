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

ExecuteCommand()