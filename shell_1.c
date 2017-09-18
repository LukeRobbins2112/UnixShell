#include	<stdlib.h>
#include	<stdio.h>
#include	<string.h>
#include	<sys/types.h>
#include	<dirent.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<ctype.h>

#define MAXLINE 1024
#define MAXARGS 128
#define		LINE_LEN	256


int inputRedirect = 0;
int outputRedirectSingle = 0;
int outputRedirectDouble = 0;

/* Function Prototypes */

void evaluteInput(char * cmdline);
int parseline(char *buf, char **argv);
int buildin_command(char **argv);
int listFiles(char * argument);

int main(){
     char cmdline[MAXLINE];

     while(1){
          write(1, "> ", 2);
          fgets(cmdline, MAXLINE, stdin);          //Read
          if (feof(stdin))
               exit(0);

          evaluteInput(cmdline);                         //Evaluate
     }
}


/* If first arg is builtin command, run it and return true */

int builtin_command(char **argv){

     if (strcmp(argv[0], "exit") == 0)    
          exit(0);
     if (strcmp(argv[0], "ln") == 0){
     	link(argv[1], argv[2]);
     	return 1;
     }
     if (strcmp(argv[0], "ls") == 0){
     	listFiles(argv[1]);
     	return 1;
     }
     if (strcmp(argv[0], "rm") == 0){
     	unlink(argv[1]);
     	return 1;
     }
     if (strcmp(argv[0], "cd") == 0){
     	chdir(argv[1]);
     	return 1;
     }
     if (!strcmp(argv[0], "&"))          //Ignore singleton &
          return 1;
     return 0;                         //Not a builtin command
}

void evaluteInput(char *cmdline){
     char *argv[MAXARGS];     //argument list execve()
     char buf[MAXLINE];      //holds modified command line
     int background;          //should the job run in the foreground or background?
     pid_t pid;               //process ID

     strcpy(buf, cmdline);
     background = parseline(buf, argv);

     if (argv[0] == NULL)
          return;               //Ignore empty lines
     if (argv[0] == '>'){
     	write(2, "Redirection error\n", 18);
     }


     if (!builtin_command(argv)){

          if ((pid = fork()) == 0){                         //Child runs user job

          		for (int i = 0; argv[i]; i++){
          			if ()
          		}


                if (execve(argv[0], argv, NULL) < 0){
                    write(1, "Command not found.", 19);
                    exit(0);
               }
          }

          /* Parent waits for foreground job to terminate */

          if (!background){
               int status;
               if (waitpid(pid, &status, 0) < 0)
                    write(1, "waitpid error\n", 22);
          }
          else{
          		write(1, pid, sizeof(pid));
          		write(1, " ", 1);
          		write(1, cmdline, strlen(cmdline));
          }
     }

     return;
}


/* Parse the command line and build the argv array */

int parseline(char *buf, char **argv){

     char *delim;          //points to first space delimiter
     int argc;          //number of args
     int background;          //is it a background job?

     buf[strlen(buf) - 1] = ' ';     //replace trailing '\n' with space -> used at end to test if buf is empty

     while(*buf && (*buf == ' '))     //ignore leading spaces
          buf++;

     /* Build the argv list */

     argc = 0;

      while ((delim = strchr(buf, ' ')) || (delim = strchr(buf, '<')) || (delim = strchr(buf, '>')) || (delim = strpbrk(buf, ">>"))){     //while there is another ' ' in buf, aka buf is not empty
     	if (delim[0] == '<' || delim[0] == '>' || delim[0] == ">>"){
     		argv[argc++] = delim[0];
     		argv[argc] = buf[strchr(buf, ' ') - buf];
     		buf = strchr(buf, ' ') + 1;
     		continue;
     	}
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


int listFiles (char * argument){


	const char*	dirPath	= (argument == NULL) ? "." : argument;
	DIR* dirPtr	= opendir(dirPath);

	if  (dirPtr == NULL)
	{
	  write(2,"Could not open ", 15);
	  write(2, dirPath, sizeof(dirPath));
	  write(2, "\n", 1);
	  exit(EXIT_FAILURE);
	}

	struct dirent*	entryPtr;
	struct stat		statBuf;
	char path[LINE_LEN];

	while  ( (entryPtr = readdir(dirPtr)) != NULL ){

	  snprintf(path,LINE_LEN,"%s/%s",dirPath,entryPtr->d_name);
	  stat(path,&statBuf);

	  if  (S_ISREG(statBuf.st_mode) ){
	  	write(1, "\t", 1);
	  	write(1, entryPtr->d_name, strlen(entryPtr->d_name));
	  	write(1, "  ", 2);
	  	write(1, statBuf.st_size, sizeof(statBuf.st_size));
	  	write(1, "\n", 1);
	  	//printf("%20s\t%u\n",entryPtr->d_name,statBuf.st_size);
	  }

	  else if  (S_ISDIR(statBuf.st_mode)){
	  	write(1, "\t", 1);
	  	write(1, entryPtr->d_name, strlen(entryPtr->d_name));
	  	write(1, "  ", 2);
	  	write(1, "(dir)\n", 6);
	  	//printf("%20s\t(dir)\n",entryPtr->d_name);
	  }
	    
	  else{
	  	write(1, "\t", 1);
	  	write(1, entryPtr->d_name, strlen(entryPtr->d_name));
	  	write(1, "  ", 2);
	  	write(1, "(other)\n", 8);
	  	//printf("%20s\t(other)\n",entryPtr->d_name);
	  }
	    
	}


	write(1, "\n", 1);

	closedir(dirPtr);
	return(EXIT_SUCCESS);
}
