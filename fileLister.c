#include	<stdlib.h>
#include	<stdio.h>
#include	<string.h>
#include	<sys/types.h>
#include	<dirent.h>
#include	<sys/stat.h>
#include	<unistd.h>

#define		LINE_LEN	256

int		main		(int	argc, char*	argv[]){
  
  const char*	dirPath	= (argc < 2) ? "." : argv[1];
  DIR*		dirPtr	= opendir(dirPath);

  if  (dirPtr == NULL)
  {
    fprintf(stderr,"Could not open %s\n",dirPath);
    exit(EXIT_FAILURE);
  }

  struct dirent*	entryPtr;
  struct stat		statBuf;
  char			path[LINE_LEN];

  while  ( (entryPtr = readdir(dirPtr)) != NULL )
  {
    snprintf(path,LINE_LEN,"%s/%s",dirPath,entryPtr->d_name);
    stat(path,&statBuf);

    if  (S_ISREG(statBuf.st_mode) )
      printf("%20s\t%u\n",entryPtr->d_name,statBuf.st_size);
    else
    if  (S_ISDIR(statBuf.st_mode) )
      printf("%20s\t(dir)\n",entryPtr->d_name);
    else
      printf("%20s\t(other)\n",entryPtr->d_name);
  }

  closedir(dirPtr);
  return(EXIT_SUCCESS);
}
