#include <stdio.h>
#include <unistd.h>


void create_pipe(int* fd){
	
	if (pipe(fd) == -1) {
            printf("Error with creating pipe\n");
        }
}


