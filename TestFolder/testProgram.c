#include <stdio.h>


void otherMethod(){
	printf("made it to test program's inner method!\n");
}

int main(){
	printf("Made it to the test program!\n");
	otherMethod();
	return 0;
}