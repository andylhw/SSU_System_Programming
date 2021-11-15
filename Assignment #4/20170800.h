#include <stdio.h>
#include <stdlib.h>

void dump_mem(const void *mem, size_t len) {
	const char *buffer = mem;
	size_t i;
	printf("Here is the mem dump!\n");
	for (i=0; i<len; i++){
		if (i>0 && i%16 == 0) {
			printf("\n");
		}
		printf("%02x ", buffer[i] & 0xff);
	}
	puts("");
}

void int_mem_allocator(const void *mem, size_t offset,int value){
	char *temp;
	int *temp2;
	temp = (char *)mem;
	temp+=offset;
	temp2 = (int*)temp;
	*temp2 = value;
	
}

void double_mem_allocator(const void *mem, size_t offset,double value){
	char *temp;
	double *temp2;
	temp = (char *)mem;
	temp+=offset;
	temp2 = (double*)temp;
	*temp2 = value;
}

void char_mem_allocator(const void *mem, size_t offset,char value){
	char *temp;
	temp = (char *)mem;
	temp+=offset;
	*temp = value;
	
}

int check_valid(size_t offset, size_t size){
	if(offset+size>256){
		printf("There is not enough memory for the data, you can only use %d byte(s)\n", 256-(int)offset);
		return -1;
	}
	//DEBUG
	//printf("offset: %ld, size: %ld\n", offset, size);
	return 0;
}

void clear_memory(const void *mem, size_t offset, size_t size){
	char* temp;
	temp = (char *)mem;
	temp+=offset;
	for(int i=0;i<size;i++){
		*temp=0;
		temp++;
	}
}
void pull_memory(const void *mem, size_t offset_start, size_t offset_end, size_t size){
	char* temp;
	temp = (char *)mem;
	for(int i=offset_start;i<offset_end-size;i++){
		*(temp+i) = *(temp+i+size);
	}
}
void rearr_memory(const void *mem, size_t offset_last, size_t len){
	int i;
	char* temp;
	temp = (char *)mem;
	temp+=offset_last;
	for(i=0;i<len;i++){
		*temp=0;
		temp++;
	}
}
void deallocator(const void *mem, size_t offset_start, size_t offset_end, size_t size){
	clear_memory(mem, offset_start, size);
	pull_memory(mem, offset_start, offset_end, size);
}
void show_memory_result(const void *mem, size_t offset){
	char* temp;
	int* temp2;
	temp = (char *)mem;
	temp+=offset;
	temp2 = (int *)temp;
	printf("Result of the int: %d\n", *temp2);
}
