#include "shell.h"
#include "builtin_commands.h"


int checkRedirection(char ** argv);
int redirectionInput(char*argv[]);
int redirectionOutput(char * argv[]);
int redirectionOutputAppend(char * argv[]);