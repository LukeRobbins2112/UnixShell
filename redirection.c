/*============================================================================
 Name        : redirection.c
 Authors     : Luke Robbins and Matthew Hurray
 ============================================================================
 */

#include "shell.h"
#include "redirection.h"
#include "builtin_commands.h"

int checkRedirection(char * buf) {

	int i = 0;
	int redirectionOp = 0;

	for (int i = 0; i < strlen(buf); i++) {
		if (buf[i] == '>' || buf[i] == '<')
			return 1;
	}
	return 0;
}

/* int isOperator(char * c){
 if (c[0] == '<')
 return 1;
 else if (c[0] == '>'){
 if (c[1] == '>')
 return 2;
 return 1;
 }

 return 0;
 }*/  //unused
int redirectionInput(char*argv[]) {

	int i = 0;
	int count = 0;
	int flag = 0;
	char file[100];
	char* msg = "You cannot have more than 1 redirection symbol here\n";
	char* msg2 = "There needs to be an input file\n";
	char* msg3 = "no file to output specified\n";

	while (argv[i] != NULL) {

		if (strcmp(argv[i], "<") == 0) {  //look for the redirection symbol
			count++;

			if (argv[i + 1] != NULL) { //check to see if there is text before the redirect, and a file to redirect output to
				flag = 1;
				strcpy(file, argv[i + 1]);

				for (; (argv[i + 2]) != NULL; i++) { //remove [redirects + input files] from argv as they arise, so execve args are correct
					argv[i] = argv[i + 2];
				}
				argv[i] = '\0';

			} else {
#ifndef NO_PROMPT
				write(1, msg3, strlen(msg3));
#endif
				return -1;
			}
		}

		i++;
	}
	if (count == 0)  //no redirection, exit method
			{
		return 0;
	}
	if (count > 1)   //more than one redirection symbol
			{
#ifndef NO_PROMPT
		write(1, msg, strlen(msg));
#endif
		return -1;
	}
	if (flag == 0)  //no input file provided
			{
#ifndef NO_PROMPT
		write(1, msg2, strlen(msg2));
#endif
		return -1;
	}

	//attempt file redirection
	int fd = open(file, O_RDONLY);
	char * errmsg = "error opening file";
	if (fd < 0) {
#ifndef NO_PROMPT
		write(1, errmsg, strlen(errmsg));
#endif
		return -1;
	} else {
		dup2(fd, 0); //setting input to the file
	}

	return fd; //success

}

int redirectionOutput(char * argv[]) {
	int i = 0;
	int count = 0;
	int flag = 0;
	char file[100];
	char* msg = "You cannot have more than 1 redirection symbol here\n";
	char* msg2 = "There needs to be an output file\n";
	char* msg3 = "no file to output specified\n";

	while (argv[i] != NULL) {

		if (strcmp(argv[i], ">") == 0) {  //look for the redirection symbol
			count++;

			if (argv[i + 1] != NULL) { //check to see if there is text before the redirect, and a file to redirect output to
				flag = 1;
				strcpy(file, argv[i + 1]);

				for (; (argv[i + 2]) != NULL; i++) { //remove [redirects + input files] from argv as they arise, so execve args are correct
					argv[i] = argv[i + 2];
				}
				argv[i] = '\0';

			} else {
#ifndef NO_PROMPT
				write(1, msg3, strlen(msg3));
#endif
				return -1;
			}
		}

		i++;
	}
	if (count == 0)  //no redirection, exit method
			{
		return 0;
	}
	if (count > 1)   //more than one redirection symbol
			{
#ifndef NO_PROMPT
		write(1, msg, strlen(msg));
#endif
		return -1;
	}
	if (flag == 0)  //no input file provided
			{
#ifndef NO_PROMPT
		write(1, msg2, strlen(msg2));
#endif
		return -1;
	}

	//attempt file redirection
	int fd = open(file, O_WRONLY | O_TRUNC | O_CREAT,
	S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
	char * errmsg = "error opening file";

	if (fd < 0) {
#ifndef NO_PROMPT
		write(1, errmsg, strlen(errmsg));
#endif
		return -1;
	} else {
		dup2(fd, 1); //sending output to the file
	}

	return fd;
}

int redirectionOutputAppend(char * argv[]) {

	int i = 0;
	int count = 0;
	int flag = 0;
	char file[100];
	char* msg = "You cannot have more than 1 redirection symbol here\n";
	char* msg2 = "There needs to be an output file\n";
	char* msg3 = "no file to output specified\n";

	while (argv[i] != NULL) {

		if (strcmp(argv[i], ">>") == 0) {  //look for the redirection symbol
			count++;

			if (argv[i + 1] != NULL) { //check to see if there is text before the redirect, and a file to redirect output to

				flag = 1;
				strcpy(file, argv[i + 1]);

				for (; (argv[i + 2]) != NULL; i++) { //remove [redirects + input files] from argv as they arise, so execve args are correct
					argv[i] = argv[i + 2];
				}
				argv[i] = '\0';

			} else {
#ifndef NO_PROMPT
				write(1, msg3, strlen(msg3));
#endif
				return -1;
			}
		}

		i++;
	}

	if (count == 0)  //no redirection, exit method
			{
		return 0;
	}

	if (count > 1)   //more than one redirection symbol
			{
#ifndef NO_PROMPT
		write(1, msg, strlen(msg));
#endif
		return -1;
	}

	if (flag == 0) {                    //no input file provided
#ifndef NO_PROMPT
		write(1, msg2, strlen(msg2));
#endif
		return -1;
	}

	//attempt file redirection
	int fd = open(file, O_WRONLY | O_APPEND,
	S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
	char * errmsg = "error opening file";

	if (fd < 0) {
#ifndef NO_PROMPT
		write(1, errmsg, strlen(errmsg));
#endif
		return -1;
	} else {
		dup2(fd, 1); //setting input to the file
	}

	return fd;
}

