#include "shell.h"
#include "redirection.h"
#include "builtin_commands.h"



// int newMethod(){

// 	//todo begin:  check file redirection.  Need to place these commands somewhere else, and maybe refactor code
    

//     int test = 0;  //set to 1 for input, 2 for output, 3 for output redirect

//     if (test == 1) {
//         int saved_stdin = dup(0);
//         int n = redirectionInput(argv);

//         if (n == 1) {
//             char * temp; //holds contents of file
//             read(0, temp, 10);  //need to figure out size of file
//             write(1, temp, strlen(temp));
//         }

//         dup2(saved_stdin, 0);
//         close(saved_stdin);
//     }

//     if (test == 2) {
//         int saved_stdout = dup(1); //save stdout to reset after file output redirection
//         int n = redirectionOutput(argv);
//         char * tempmsg = "file redirection input successful";
//         if (n == 1) {
//             write(1, tempmsg, strlen(tempmsg));
//         }
//         /* Restore output */
//         dup2(saved_stdout, 1);
//         close(saved_stdout);
//     }

//     if (test == 3) {
//         int saved_stdout = dup(1); //save stdout to reset after file output redirection
//         int n = redirectionOutputAppend(argv);
//         char * tempmsg = "appending this message to the file";
//         if (n == 1) {
//             write(1, tempmsg, strlen(tempmsg));
//         }
//         /* Restore output */
//         dup2(saved_stdout, 1);
//         close(saved_stdout);
//     }

//     //todo test end:
// }


int checkRedirection(char * buf) {

	//todo: get more specific with this. I was thinking either (A) return the count of redirect operators and go from there 
	//or (B) return a specific flag number that corresponds to different situations
	//e.g. if (just input redirection) {return 1}, else if (input and output redirect){return 2} etc. etc.
	//might also be a good idea to use constants e.g. INPUT_AND_OUTPUT = 3


    int i = 0;
    int redirectionOp = 0;

    for (int i = 0; i < strlen(buf); i++){
    	if (buf[i] == '>' || buf[i] == '<')
    		return 1;
    }
    return 0;
}

// int isOperator(char * c){
// 	if (c[0] == '<')
// 		return 1;
// 	else if (c[0] == '>'){
// 		if (c[1] == '>')
// 			return 2;
// 		return 1;
// 	}

// 	return 0;
// }


int redirectionOutput(char*argv[]) {

    int i = 0;
    int count = 0;
    int flag = 0;
    char*file;
    char* msg = "You cannot have more than 1 redirection symbol here\n";
    char* msg2 = "There needs to be an input file\n";

    while (argv[i] != NULL) {

        if (strcmp(argv[i], ">") == 0) {  //look for the redirection symbol
            count++;
            if (argv[i + 1] != NULL) { //check to see if there is text after the redirect
                flag = 1;
                file = argv[i + 1];  //copy the name of the file
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
        write(1, msg, strlen(msg));
        return -1;
    }
    if (flag == 0)  //no input file provided
            {
        write(1, msg2, strlen(msg2));
        return -1;
    }

    //attempt file redirection
    int fd = open(file, O_RDONLY);
    char * errmsg = "error opening file";
    if (fd < 0) {
        write(1, errmsg, strlen(errmsg));
        return -1;
    } else {
        dup2(fd, 0); //setting input to the file
    }

    close(file);
    return 1; //success
}


int redirectionInput(char * argv[]) {
    int i = 0;
    int count = 0;
    int flag = 0;
    char*file;
    char* msg = "You cannot have more than 1 redirection symbol here\n";
    char* msg2 = "There needs to be an output file\n";

    while (argv[i] != NULL) {

        if (strcmp(argv[i], "<") == 0) {  //look for the redirection symbol
            count++;

            if (i > 0) { //check to see if there is text before the redirect
                flag = 1;
                file = argv[i - 1];  //copy the name of the file
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
        write(1, msg, strlen(msg));
        return -1;
    }
    if (flag == 0)  //no input file provided
            {
        write(1, msg2, strlen(msg2));
        return -1;
    }

    //attempt file redirection
    int fd = open(file, O_WRONLY | O_TRUNC | O_CREAT,
    S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
    char * errmsg = "error opening file";

    if (fd < 0) {
        write(1, errmsg, strlen(errmsg));
        return -1;
    } 
    else {
        dup2(fd, 1); //setting input to the file
    }

    close(file);
    return 1;
}


int redirectionOutputAppend(char * argv[]) {

    int i = 0;
    int count = 0;
    int flag = 0;
    char*file;
    char* msg = "You cannot have more than 1 redirection symbol here\n";
    char* msg2 = "There needs to be an output file\n";

    while (argv[i] != NULL) {

        if (strcmp(argv[i], ">>") == 0) {  //look for the redirection symbol
            count++;

            if (i > 0) { //check to see if there is text before the redirect
                flag = 1;
                file = argv[i - 1];  //copy the name of the file
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
        write(1, msg, strlen(msg));
        return -1;
    }

    if (flag == 0) {                    //no input file provided
        write(1, msg2, strlen(msg2));
        return -1;
    }

    //attempt file redirection
    int fd = open(file, O_WRONLY | O_APPEND,
    S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
    char * errmsg = "error opening file";

    if (fd < 0) {
        write(1, errmsg, strlen(errmsg));
        return -1;
    } else {
        dup2(fd, 1); //setting input to the file
    }

    close(file);
    return 1;
}