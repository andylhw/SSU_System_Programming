#include "20170800.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
// An example of a semphore for the mutual exclusion
semaphore_t mutex;
//to read.
char str[3][1024]= {0};
FILE *fp;
//to make it end when finish reading lines.
int done = 1;

void reader(void)
{ 
	//Reading until variable 'done' changes
	while(done){
		//to lock
		P(&mutex);
		int i=get_thread_id()-1;
		//thread i-> save to str[i],
		if(fgets(str[i], 1024, fp) == NULL){
			//if read all lines-> done will be 0 to make while loop break.
			done=0;
			printf("Thread %d: read all line!\n", get_thread_id());
			return;
		}
		printf("Thread %d is reading a line...\n", get_thread_id());
		//to unlock
		V(&mutex);
	}
	// your code
}

void writer(void)
{
	while(done){
		//lock
		P(&mutex);
		printf("Thread %d is writing 3 lines\n", get_thread_id());
		//to write everything in str[]
		for(int i=0;i<3;i++){
			printf("%s", str[i]);
		}
		//unlock
		V(&mutex);
	}
	// your code
}

int main(int argc, char *argv[])
{
    //printf("20170800 Lee Hwawon Assignment #6\n");
    if (argc != 2 ){
    	printf("Usage: 20170800 <file>\n");
	return 0;
    }
    fp = fopen(argv[1],"r");
    if(fp==NULL){
    	printf("Error: Unable to Open %s\n", argv[1]);
    }
    // example of initializing a semaphore
    mutex = sem_create(1);
    // your code
    
    mutex.sem_q = (tcb_t*)malloc(sizeof(tcb_t));
    //to make Queue working. -> malloc the queue.
    thread_queue = (tcb_t*)malloc(sizeof(tcb_t));
    
    // create threads
    create_thread(reader);
    create_thread(reader);
    create_thread(reader);
    create_thread(writer);
    
    // Let's run
    run();
    
    // stupid while loop
    while (1) {
    	sleep(1);
    }
}   




