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

fork() : creates a new child process by duplicating the current process (parents). child return 0 , child pid in parent, -1 on faliure
execvp() :Replaces the child process with a new program ex ls,ps
file descriptor(): A small integer used by UNIX to respresent open input/output stream
    stdin = fd 0 keyboard by default
    stdout = fd 1 terminal by default
    stderr = fd 2 error ooutput 
dup2(oldfd, newfd) : redirect I/O by making newfd point to same place as oldfd to connect a file stdin(0) or stdout(2) for < and >
waitpid() : used by the parent shell to wait for the child process to finish , so the prompt returns only after the command completes (foregound behaior)
errno : global error code set by system call when something fials 
strerror(errno): converts errno into readable error message string

    ExecuteCommand() runs whatever command the user typed after it has been pasred into ShellCommand struct
    1. command is empty args == NULL do nothing
    2. command is exit it returns 1 to main stopping the loop and terminating the shell
    3. command is cd  it calls chdir() in the parents process so shell changes directory
    
    4.for command (ls ps )
    The shell calls fork() creating child process
        in child :
            if < file is provided it opens file and uses dup2() to redirect it to stdin (fd0)
            if > file is provided it opens/creats the file and use dup2() to redirect to stdout (fd1).
            now after the command is redirected according it runs the program using execvp(args[0],args)
            if execvp fails it prints an error using errno + strerror and exit the child 
    
   5. in parents:
         it waits for the child to finish using waitpid() so the prompt doesnt return early 