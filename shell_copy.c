#include    <stdlib.h>
#include    <stdio.h>
#include    <string.h>
#include    <sys/types.h>
#include    <dirent.h>
#include    <sys/stat.h>
#include    <unistd.h>
#include    <ctype.h>
#include    <fcntl.h>
#define MAXLINE 1024
#define MAXARGS 128
#define     LINE_LEN    256


/* Function Prototypes */
void eval(char * cmdline);
int parseline(char *buf, char **argv);
int buildin_command(char **argv);
int listFiles(char * argument);
void ls();
char* getInput(char input[]);
void cd(char ** tokens);
int checkRedirection(char ** argv);


int main() {

    char cmdline[MAXLINE];
    while (1) {
        write(1, "> ", 2);
        getInput(cmdline);
        if (feof(stdin))
            exit(0);
        eval(cmdline);                         //Evaluate
    }
}

char* getInput(char input[]) {
    char cwd[MAXLINE];
    getcwd(cwd, sizeof(cwd));  //should I null safe?
    opendir(cwd);
    char *ind = ":~$ ";
    char *msg = strcat(cwd, ind);
    close(cwd);
    write(1, msg, strlen(msg));
    memset(input, 0, sizeof(input));  //clearing buffer of input
    int in = read(0, input, MAXLINE);
    //input[strcspn(input, "\n")] = 0;  //remove trailing new line
    return input;
}

/* If first arg is builtin command, run it and return true */
int builtin_command(char **argv) {
    if (strcmp(argv[0], "exit") == 0) {
        write(1, "bye", 3);  //todo:  discuss message for shell shutdown
        exit(0);
    }
    if (strcmp(argv[0], "ln") == 0) {
        link(argv[1], argv[2]);
        return 1;
    }
    if (strcmp(argv[0], "ls") == 0) {
        //listFiles(argv[1]);   //  todo:  discuss functionality here
        ls();
        return 1;
    }
    if (strcmp(argv[0], "rm") == 0) {
        unlink(argv[1]);
        return 1;
    }
    if (strcmp(argv[0], "cd") == 0) {
        //chdir(argv[1]);
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


int checkRedirection(char *argv[]) {
    int i = 0;
    while (argv[i] != NULL) {
        if (strcmp(argv[i], ">") == 0) {
            return 1;
        }
        if (strcmp(argv[i], "<") == 0) {
            return 1;
        }
        if (strcmp(argv[i], "<<") == 0) {
            return 1;
        }
        i++;
    }
    return 0;
}


void eval(char *cmdline) {
    char *argv[MAXARGS];     //argument list execve()
    char buf[MAXLINE];      //holds modified command line
    int background;        //should the job run in the foreground or background?
    pid_t pid;               //process ID
    strcpy(buf, cmdline);

    background = parseline(buf, argv);

    if (argv[0] == NULL)
        return;               //Ignore empty lines
    //todo:  maybe move this checkRedirection call
    if (checkRedirection(argv)==1)
    {
        write(1,"success\n",8);  //todo:  need to deal with redirection
    }
        if (!builtin_command(argv)) { //todo:  made a logic change, I think it was reversed before
            if ((pid = fork()) == 0) {                     //Child runs user job
                if (execve(argv[0], argv, 0) < 0) {
                    write(1, "Command not found.", 19);
                    exit(0);
                }
            }
            // Parent waits for foreground job to terminate
            if (!background) {
                int status;
                if (waitpid(pid, &status, 0) < 0)
                    write(1, "waitfg: waitpid error\n", 22);
            } else {
                write(1, pid, sizeof(pid));
                write(1, " ", 1);
                write(1, cmdline, sizeof(cmdline));
            }
        }
        return;
    }
    

/* Parse the command line and build the argv array */

int parseline(char *buf, char **argv) {

        char *delim;          //points to first space delimiter
        int argc;          //number of args
        int background;          //is it a background job?
        buf[strlen(buf) - 1] = ' '; //replace trailing '\n' with space -> used at end to test if buf is empty

        while (*buf && (*buf == ' '))     //ignore leading spaces
            buf++;
        
        /* Build the argv list */

        argc = 0;

        while ((delim = strchr(buf, ' '))) { //while there is another ' ' in buf, aka buf is not empty
            argv[argc++] = buf;
            *delim = '\0';
            buf = delim + 1;
            while (*buf && (*buf == ' '))     //Ignore spaces
                buf++;
        }
        argv[argc] = NULL;
        if (argc == 0)     //Ignore blank line
            return 1;
        /* Should the job run in the background? */
        if ((background = (*argv[argc - 1] == '&')) != 0)
            argv[--argc] = NULL;
        return background;
    }


int listFiles(char * argument) {
        const char* dirPath = (argument == NULL) ? "." : argument;
        DIR* dirPtr = opendir(dirPath);
        if (dirPtr == NULL) {
            write(2, "Could not open ", 15);
            write(2, dirPath, sizeof(dirPath));
            write(2, "\n", 1);
            exit(EXIT_FAILURE);
        }
        struct dirent* entryPtr;
        struct stat statBuf;
        char path[LINE_LEN];
        while ((entryPtr = readdir(dirPtr)) != NULL) {
            snprintf(path, LINE_LEN, "%s/%s", dirPath, entryPtr->d_name);
            stat(path, &statBuf);
            if (S_ISREG(statBuf.st_mode)) {
                write(1, "\t", 1);
                write(1, entryPtr->d_name, strlen(entryPtr->d_name));
                write(1, "  ", 2);
                write(1, statBuf.st_size, sizeof(statBuf.st_size));
                write(1, "\n", 1);
                //printf("%20s\t%u\n",entryPtr->d_name,statBuf.st_size);
            }
            else if (S_ISDIR(statBuf.st_mode)) {
                write(1, "\t", 1);
                write(1, entryPtr->d_name, strlen(entryPtr->d_name));
                write(1, "  ", 2);
                write(1, "(dir)\n", 6);
                //printf("%20s\t(dir)\n",entryPtr->d_name);
            }
            else {
                write(1, "\t", 1);
                write(1, entryPtr->d_name, strlen(entryPtr->d_name));
                write(1, "  ", 2);
                write(1, "(other)\n", 8);
                //printf("%20s\t(other)\n",entryPtr->d_name);
            }
        }
        write(1, "\n", 1);
        closedir(dirPtr);
        return (EXIT_SUCCESS);
    }


//method lists directory contents
void ls() {
        char cwd[MAXLINE];
        getcwd(cwd, sizeof(cwd));  //should I null safe?
        DIR *dir = opendir(cwd);
        struct dirent *dirent;
        if (dir == NULL) {
            write(1, "null", 4);
        } else {
            while (dirent = readdir(dir)) {
                if (dirent->d_name[0] != '.')  //ignore hidden files
                        {
                    char *fname = dirent->d_name;
                    write(1, strcat(fname, "\n"), strlen(dirent->d_name) + 1);
                }
            }
        }
        closedir(dir);
}


//method traverses file directory
void cd(char** tokens) {
        char * noInputMsg = "argument expected after command \"cd\"\n";
        char * noDirMsg = "Cannot find directory\n";
        //check first token
        char * first = tokens[1];
        first[strcspn(first, "\n")] = 0;  //remove trailing new line
        if (first == NULL) {
            write(1, noInputMsg, strlen(noInputMsg));
            return;
        }
        //go back a level
        if (strcmp(first, "..") == 0) {
            chdir("..");
            return;
        }
        if (chdir(first) != 0) {
            write(1, noDirMsg, strlen(noDirMsg));
        } else {
            chdir(first);
        }
    }


//method executes a file or program
void cat(char** tokens) {
    char * filename = tokens[1];
    filename[strcspn(filename, "\n")] = 0;  //remove trailing new line
    int fd = open(filename, O_RDONLY);
    off_t fileLength = lseek(fd, 0, SEEK_END);  // goes to end of file
    lseek(fd, 0, SEEK_SET); //set buffer to beginning
    int bytes;
    char content[fileLength];
    
    if (fd < 0) {
            write(1, "error", 5);
        } else {
            bytes = read(fd, content, fileLength);
            write(1, content, bytes);
        }
        close(fd);
}




