
/*
    This program defines a program that parses a path into its component entries.  
*/

#include	<stdlib.h>
#include	<stdio.h>
#include	<string.h>
#include	<ctype.h>

#define		LINE_LEN	256

struct	DirEntryName
{
  char*			name_;
  struct DirEntryName*	nextPtr_;
};


struct	PathName
{
  int			isRoot_;
  int			isMyHome_;
  int			isSomeonesHome_;
  struct DirEntryName*	dirEntryNamePtr_;
};


//  return a non-zero value if 'c' is legal in a restricted Unix directory entry name, or '0' otherwise.

int	isLegalDirEntryChar (char		c){
  return( isalnum(c) || (c == '-') || (c == '_') || (c == '.') );
}


//  return 'argv[1]' if there is at least one argument on the
//	command line according to 'argc'.  Otherwise:
//	(1) asks the user for a path,
//	(2) enters it into 'textSpace' of length 'textSpaceLen',
//	(3) removes the ending '\n' from 'textSpace',
//	(4) and returns 'textSpace'.

const char*	getPathText	(int argc, char*	argv[], char* textSpace, int textSpaceLen){

  if (argc >= 1){
      return argv[1];
  }

  write(1, "Please enter the path name\n", 27);
  
  fgets(textSpace, textSpaceLen, stdin);
  char * endChar = strchr(textSpace, '\n');
  (*endChar) = '\0';

  return textSpace;

}


//  PURPOSE:  To create and return a linked list of heap-allocated struct DirEntryName instances that represent the path 'linePtr'. 

//	(1) If 'linePtr' is empty (points to '\0') then returns 'NULL'.
//	(2) If 'linePtr' is not empty then it
//	    (a) allocates a 'struct DirEntryName' instance from the heap,
//	    (b) allocates a C-string to hold the directory entry 'linePtr'
//	    	points to.
//	    (c) allocates a new 'struct DirEntryName' for the next directory
//	    	entry, etc.
//
//	Directory entries are separated by the '/' char.


struct DirEntryName* parseRestOfPath	(const char*	linePtr){


  if  (linePtr == NULL)
  {
    write(2 ,"NULL ptr to parseRestOfPath()\n", 30);    
    exit(EXIT_FAILURE);
  }


  //  Handle when at end of 'linePtr':

  if (*linePtr == '\0'){
    return NULL;
  }


  // Handle when 'linePtr' points to '//', and thus a missing directory name:

    if  ((*linePtr) == '/'){
        write(2, "Missing directory name!\n", 24);
        exit(EXIT_FAILURE);
    }

  //  Get entry name:

  char*	entryNamePtr	= linePtr;

  //  Leave 'entryNamePtr' pointing to the beginning of the entry name 
  //  advance 'linePtr' until the characters are no longer legal

    while (isLegalDirEntryChar(*linePtr)){
        linePtr++;
    }


  //  If stopped because of anything other than '/' or '\0' then the user gave  an illegal char

    if ((*linePtr != '/') && (*linePtr != '\0')){
        write(2, "Illegal character ", 18);
        write(2, *linePtr, 1);
        write(2, "in path!\n", 9);
        exit(EXIT_FAILURE);
    }



  //   Allocate a new 'struct DirEntryName*' pointer here.
  //	 Allocate memory for its name and copy entry name into that mem:

    struct DirEntryName * newDir = (struct DirEntryName *)malloc(sizeof(struct DirEntryName));
    newDir->name_ = (char *)malloc(sizeof(linePtr - entryNamePtr + 1));
    strncpy(newDir->name_, entryNamePtr, (linePtr - entryNamePtr));
    *(newDir->name_ + (linePtr - entryNamePtr + 1)) = '\0';


  //  If 'linePtr' encountered '/' it should get the value for
  //  'nextPtr_' by recursion.  If it points to '\0' it should set'nextPtr_' to 'NULL'.

    if (*linePtr == '/'){
        newDir->nextPtr_ = parseRestOfPath(linePtr+1);
    }
    if (*linePtr == '\0'){
        newDir->nextPtr_ = NULL;
    }

    return newDir;

}


//  Return the address of a heap-allocated 'struct PathName'
//	instance that encodes the path given by 'linePtr'.

struct PathName* getPath (const char*	linePtr){

  if  (linePtr == NULL)
  {
    write(2, "NULL ptr to getPath()!\n", 23);
    exit(EXIT_FAILURE);
  }


  // Create 'struct PathName' object, obtain heap memory: 

  struct PathName*	toReturn	= (struct PathName*)malloc(sizeof(struct PathName));


  // Initialize flags of '*toReturn':

  toReturn->isRoot_			= 0;
  toReturn->isMyHome_			= 0;
  toReturn->isSomeonesHome_		= 0;

  switch  (*linePtr){

        case '/' :
          toReturn->isRoot_	= 1;
          linePtr++;
          break;

        case '~' :
          linePtr++;

          if  (*linePtr == '\0')
            toReturn->isMyHome_	= 1;
          else
          if  (*linePtr == '/')
          {
            toReturn->isMyHome_	= 1;
            linePtr++;
          }
          else
            toReturn->isSomeonesHome_	= 1;

          break;

        case '\0' :
          write(2,"Empty path!\n", 12);
          exit(EXIT_FAILURE);
  }

  toReturn->dirEntryNamePtr_	= parseRestOfPath(linePtr);

  return(toReturn);
}


//  To print out the constructed path '*pathNamePtr'. 

void	print	(struct PathName* pathNamePtr){

  if  (pathNamePtr == NULL)
  {
    write(2, "NULL ptr to print()!\n", 21);
    exit(EXIT_FAILURE);
  }


  int	sum	= pathNamePtr->isRoot_	+ pathNamePtr->isMyHome_	+ pathNamePtr->isSomeonesHome_;

  if  ( (sum < 0) || (sum > 1) )
  {
     write(2, "Inconsistent pathname!\n", 23);
     exit(EXIT_FAILURE);
  }

  struct DirEntryName*	run	= pathNamePtr->dirEntryNamePtr_;

  if  (pathNamePtr->isRoot_){
     write(1, "Start from the root directory /\n", 32);
  }
  else if  (pathNamePtr->isMyHome_){
    write(1, "Start from your home directory ~\n", 33);
  }
  else if  (pathNamePtr->isSomeonesHome_)
  {
    write(1, "Start from the home directory of ", 33);
    write(1, run->name_, sizeof(run->name_));
    run = run->nextPtr_;
  }
  else{
    write(1, "Start from current directory\n", 29);
  }

  for  ( ;  run != NULL;  run = run->nextPtr_)
    write(1, run->name_, sizeof(run->name_));

}

//  To 'free()' the memory of 'pathNamePtr': all DirEntryName
//	'name_' and 'nextPtr_' member vars, and the memory of 'pathNamePtr' itself. 

void	destroy	(struct PathName* pathNamePtr){

    struct DirEntryName*  nextPtrCopy = pathNamePtr->dirEntryNamePtr_;
    struct DirEntryName* DEtoFree;

    while (nextPtrCopy  != NULL){
      DEtoFree = nextPtrCopy;
      nextPtrCopy = nextPtrCopy->nextPtr_;          //Keep copy because we're deleting nextPtr references
      free(DEtoFree);	
    }


    free(nextPtrCopy);            //delete copy reference pointer
    free(pathNamePtr);          //delete PathName pointer
}




int		main		(int		argc, char*		argv[]){

  char	textSpace[LINE_LEN];
  const char*		linePtr	= getPathText(argc,argv,textSpace,LINE_LEN);
  struct PathName*	pathPtr	= getPath(linePtr);

  print(pathPtr);
  destroy(pathPtr);
  return(EXIT_SUCCESS);
}






