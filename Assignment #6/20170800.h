#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ucontext.h>

#define STACK_SIZE 8192

typedef struct tcb_t {
	struct tcb_t *next;
	struct tcb_t *prev;
	ucontext_t   context;
	int threadid;
}tcb_t;
tcb_t *thread_queue = 0;
tcb_t *curr_thread = 0;
int g_tid = 1;

// addQueue(head, item) adds a queue item, pointed to by “item”, to the queue pointed to by head.
void addQueue(tcb_t **head, tcb_t *item) {
	// your code
	tcb_t *queue = *head;
	if (queue->prev == NULL){ // if init
		queue->next = item;
		queue->prev = item;
	}
	else{
		queue->prev->next = item;
		queue->prev = item;
	/* for DEBUG
	printf("queue->next threadid: %d\n", queue->next->threadid);
	printf("queue->next-> threadid: %d\n", queue->next->next->threadid);
	printf("queue->last threadid: %d\n", queue->prev->threadid);
	*/
	}
}

// delQueue(head) deletes an item from head and returns a pointer to the deleted item. If the queue is already empty, flag error.
tcb_t *delQueue(tcb_t **head) {
	// your code
	if((*head)-> next == NULL){
		//flag error
		return 0;
	}
	else{
		tcb_t * tcb = (*head)->next;
		(*head)->next = tcb->next;
		return tcb;
	}

}


void init_TCB (tcb_t *tcb, void *function, void *stackP, int stack_size) {
	memset(tcb,'\0', sizeof(tcb_t));
	getcontext(&tcb->context);
	tcb->context.uc_stack.ss_sp = stackP;
	tcb->context.uc_stack.ss_size = (size_t) stack_size;
	makecontext(&tcb->context,function,0);
}


void create_thread(void (*function)(void)) {
	void *stack = (void *)malloc(STACK_SIZE);
	tcb_t *tcb = (tcb_t *)malloc(sizeof(tcb_t));
	init_TCB(tcb, function, stack, STACK_SIZE);
	tcb->threadid = g_tid++;
	addQueue(&thread_queue, tcb);
}

void run() {
	curr_thread = delQueue(&thread_queue);
	ucontext_t parent; 
	getcontext(&parent); // magic sauce
	swapcontext(&parent, &(curr_thread->context)); // start the first thread
}

void yield() {  
	tcb_t *prev_thread;
	addQueue(&thread_queue, curr_thread); 
	prev_thread = curr_thread;
	curr_thread = delQueue(&thread_queue);
	//swap the context, from prev_thread to the thread pointed to curr_thread
	swapcontext(&(prev_thread->context), &(curr_thread->context));
}

int get_thread_id() {
	return curr_thread->threadid;
}



/* Semaphore Implementation */

typedef struct semaphore_t {
	int counter;
	tcb_t *sem_q;
} semaphore_t;


// mallocs a semaphore structure, initializes it to the value and returns the pointer to the semaphore.
semaphore_t sem_create(int value) {
	semaphore_t sem_test;
	
	sem_test.counter = value;
	sem_test.sem_q = NULL;
	return sem_test;
}

// decrements the semaphore. 
// if the value is <= 0, then blocks the thread in the queue associated with the semaphore.
void P(semaphore_t *sem) {
	sem->counter--;
	
	if(sem->counter<=0){
		addQueue(&sem->sem_q, curr_thread);
		
	}else{
		yield();
	}
	// your code
}

// increments the semaphore.
// if the value => 1, then takes a TCB out of the semaphore queue and puts it into the thread_queue. 
// Note: The V routine also "yields" to the next runnable thread.
void V(semaphore_t *sem) {
	sem->counter++;
	// your code
	if(sem->counter>=1){
		curr_thread = delQueue(&sem->sem_q);
		yield();
	}
}
