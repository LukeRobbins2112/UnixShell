# UnixShell

This implementation of a Unix shell has two main purposes: to execute user programs and to run typical shell commands.

The goal of this project was to write a Unix shell in C using only system calls and a
limited selection of various library functions. The submitted shell has a multitude of built-in
functions, can handle redirection, and is able to execute programs with a list of arguments
supplied.

The submitted shell has the following built-in commands: cd, ls, cat, ln, and rm. 

The current working directory is displayed at every prompt using the system call getcwd(). Calling
‘ls’ lists all visible directories and files. Calling “cd ..” will move the directory up one level, and
calling “cd dir” will move the current working directory into that subdirectory. Calling “cd” or “cd
incorrect-directory” will not work and appropriate feedback is displayed. Calling “cat” displays
the content of the file using system calls of open() and read(). Calling “ln” and “rm” behave as
expected using the system calls of link() and unlink().

File Redirection

The file redirection symbols >, <, and << are all handled by the shell. Since
redirection can have many permutations (including errors), error handling was implemented:
    -The shell is able to handle whether or not spaces are included on redirect inputs (i.e.
    “cat>bar<README” is handled the same as “cat > bar < README”). 
    -Multiple identical redirect
    -symbols are not allowed, and redirecting to a missing file is also not allowed.

After parsing for built-in commands and file redirection, the shell next looks to run an
executable. If the parsed statement is not an executable program, “command not found” is
displayed and the user is return to the prompt. If an executable is found, then the program will
fork() and execve() that program. The shell will wait for the termination of that program, and
then will continue with another prompt.

The input buffer (i.e. keyboard) is limited to 1024 bytes. Additionally, shell operations such as 
history and tab completion are not available. 

One of the first problems that needed to be dealt with was buffer problems using the read() and write() system calls. The initial resolution was to use memset, but it did not resolve buffer issues of successive inputs that were larger than the last.
In the end, the shell uses malloc and freed the memory for the user input to resolve this issue.
 
