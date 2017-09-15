# UnixShell

This implementation of a Unix shell has two main purposes: to execute user programs and to run typical shell commands.

The user is presented with a shell prompt ">", and their input is parsed to determine what they would like to do. 
    Built-in commands are run as they would be in bash. These include ls, cd, rm, ln, and exit.
    Program files can be submitted with absolute or relative pathnames, and if the requested file exists, it is run.
    
This shell also features I/O redirection with the "<", ">", and ">>" operators (the pipe operator "|" is not supported). 

In the future, the list of available built-in commands will be expanded, as well as signal handling for the user pressing CTRL-D. 
    
