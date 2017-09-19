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


void eval(char * cmdline);
int parseline(char *buf, char **argv);
int builtin_command(char **argv);
int listFiles(char * argument);
void ls();
char* getInput(char input[]);
void cd(char ** tokens);
int checkRedirection(char * buf);