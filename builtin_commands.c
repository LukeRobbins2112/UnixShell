/*============================================================================
 Name        : buildin_commands.c
 Authors     : Luke Robbins and Matthew Hurray
 ============================================================================
 */
#include "shell.h"
#include "redirection.h"
#include "builtin_commands.h"

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
		} else if (S_ISDIR(statBuf.st_mode)) {

			write(1, "\t", 1);
			write(1, entryPtr->d_name, strlen(entryPtr->d_name));
			write(1, "  ", 2);
			write(1, "(dir)\n", 6);


			//printf("%20s\t(dir)\n",entryPtr->d_name);
		} else {

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
	getcwd(cwd, sizeof(cwd));
	DIR *dir = opendir(cwd);
	struct dirent *dirent;

	if (dir == NULL) {
		
		write(1, "null", 4);
	} 
	else {
		while (dirent = readdir(dir)) {
			if (dirent->d_name[0] != '.') {            //ignore hidden files
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
	if (first == NULL) {
		write(1, noInputMsg, strlen(noInputMsg));
		return;
	}

	first[strcspn(first, "\n")] = 0;  //remove trailing new line

	//go back a level
	if (strcmp(first, "..") == 0) {
		chdir("..");
		return;
	}
	if (chdir(first) != 0) {
		write(1, noDirMsg, strlen(noDirMsg));
	} 
	else {
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
	} 
	else {
		bytes = read(fd, content, fileLength);
		write(1, content, bytes);
	}

	close(fd);
}
