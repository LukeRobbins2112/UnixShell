/*============================================================================
 Name        : shell3.c
 Authors     : Luke Robbins and Matthew Hurray
 ============================================================================
 */

#include "shell.h"
#include "redirection.h"
#include "builtin_commands.h"

int main() {
    while (1) {
        char * cmdline = malloc(MAXLINE * sizeof(char));
        getInput(cmdline);

        if (feof(stdin))
            exit(0);
        eval(cmdline);                         //Evaluate

        free(cmdline);
    }
}

char* getInput(char input[]) {
    char cwd[MAXLINE];
    getcwd(cwd, sizeof(cwd));
    DIR * dPtr = opendir(cwd);
    char *ind = ":~$ ";
    char *msg = strcat(cwd, ind);
    close(dPtr);

#ifndef NO_PROMPT
    write(1, msg, strlen(msg));
#endif

    memset(input, 0, sizeof(input));  //clearing buffer of input
    int in = read(0, input, MAXLINE);

    return input;
}

void eval(char *cmdline) {

    char *argv[MAXARGS];     //argument list execve()
    char *buf = malloc(MAXLINE * sizeof(char) + (4 * sizeof(char)));

    int fds[3];
    int background;
    pid_t pid;

    strcpy(buf, cmdline);
    background = parseline(buf, argv);

    if (argv[0] == NULL)
        return;               //Ignore empty lines

    if (!builtin_command(argv)) {

        if ((pid = fork()) < 0) {
            write(1, "Process creation failed\n", 24);
        }

        if (pid == 0) {                        //Child runs user job
            fds[0] = redirectionInput(argv);
            fds[1] = redirectionOutput(argv);
            fds[2] = redirectionOutputAppend(argv);

            argv[0][strcspn(argv[0], "\n")] = 0; //remove trailing new line from 1st argument

            if (execve(argv[0], argv, 0) < 0) {
#ifndef NO_PROMPT
                write(1, "Command not found.\n", 20);
#endif
                exit(0);
            }

        }

        // Parent waits for foreground job to terminate
        if (!background) {
            int status;
            if (waitpid(pid, &status, 0) < 0) {
#ifndef NO_PROMPT
                write(1, "waitfg: waitpid error\n", 22);
#endif

            }
        } else {
#ifndef NO_PROMPT
            write(1, pid, sizeof(pid));
            write(1, " ", 1);
            write(1, cmdline, strlen(cmdline));
#endif
        }

        for (int i = 0; i < 3; i++) {
            if (fds[i] > 0)
                close(fds[i]);
        }
    }
    free(buf);
}

/* Parse the command line and build the argv array */
int parseline(char *buf, char **argv) {

    char *delim;          //points to first space delimiter
    int argc;
    int background;
    char spacedBuf[MAXLINE + 4]; //Room for whole line + 2 spaces (assuming two redirects max)
    int spacedBufPosition = 0;

    //Creating a buffer that adds spaces around redirection to ensure proper parsing

    if (checkRedirection(buf)) {
        for (int i = 0; i < strlen(buf); i++) {
            if (buf[i] == '<') {
                spacedBuf[spacedBufPosition++] = ' ';
                spacedBuf[spacedBufPosition++] = buf[i];
                spacedBuf[spacedBufPosition++] = ' ';
            } else if (buf[i] == '>') {
                spacedBuf[spacedBufPosition++] = ' ';
                spacedBuf[spacedBufPosition++] = buf[i];

                if (buf[i + 1] == '>')                           //for double >>
                    spacedBuf[spacedBufPosition++] = buf[i + 1];

                spacedBuf[spacedBufPosition++] = ' ';
            } else {
                spacedBuf[spacedBufPosition++] = buf[i];
            }
        }
        strncpy(buf, spacedBuf, sizeof(spacedBuf));
    }

    buf[strlen(buf) - 1] = ' '; //replace trailing '\n' with space -> used at end to test if buf is empty

    while (*buf && (*buf == ' '))     //ignore leading spaces
        buf++;

    /* Build the argv list */
    argc = 0;
    while ((delim = strchr(buf, ' '))) { //while there is another ' ' in buf, aka buf is not empty
        *delim = '\0';
        argv[argc++] = buf;

        // printf("%s\n", argv[argc-1]);       //printfs out argv element by element, for debugging

        buf = delim + 1;
        while (*buf && (*buf == ' '))     //Ignore spaces
            buf++;
    }

    argv[argc] = NULL;          //Null terminate the argument list
    if (argc == 0)     //Ignore blank line
        return 1;

    if ((background = (*argv[argc - 1] == '&')) != 0)
        argv[--argc] = NULL;

    return background;
}

/* If first arg is builtin command, run it and return true */
//Ignores other command line arguments
int builtin_command(char **argv) {
    if (strcmp(argv[0], "exit") == 0) {
#ifndef NO_PROMPT
        write(1, "bye\n", 4);
#endif
        exit(0);
    }
    if (strcmp(argv[0], "ln") == 0) {
        link(argv[1], argv[2]);
        return 1;
    }
    if (strcmp(argv[0], "ls") == 0) {
        //listFiles(argv[1]);   //alternate ls function
        ls();
        return 1;
    }
    if (strcmp(argv[0], "rm") == 0) {
        unlink(argv[1]);
        return 1;
    }
    if (strcmp(argv[0], "cd") == 0) {
        //chdir(argv[1]);  //basic change directory
        cd(argv);
        return 1;
    }
    if (strcmp(argv[0], "cat") == 0) {
        cat(argv);
        return 1;
    }
    if (!strcmp(argv[0], "&"))          //Ignore singleton &
        return 1;

    return 0;                         //Not a builtin command
}

//todo: handle CTRL-D with signal handler

