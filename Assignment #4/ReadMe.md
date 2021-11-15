Assignment 4 - Memory allocator
=============

256 byte인 Memory가 존재한다. 해당 메모리에 다양한 값을 집어넣어서 메모리를 저장시킬 수 있게하라
>1. char
>2. int
>3. double
>4. struct

소스코드
============
20170800.c
--------------
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "20170800.h"

#define MAX_MEM 256

int main(){
	int *numPtr2;
	int choice = 0;
	int input_int=0;
	double input_double=0;
	char input_char;
	char sec_choice[]="";
	char third_choice[]="";
	//뭔 입력을 받을건지 기록해놓자.
	char *choice2[]={"int", "double", "struct", "char"};
	//이름과 offset, size를 저장을 위한 함수. char256개 저장을 대비해서 최대공간 256.
	int box_offset[MAX_MEM+1] = {0};
	int box_size[MAX_MEM]={0};
	char box_name[MAX_MEM][30] = {""};
	//몇개가 쌓였는지 확인.
	int count=1;
	int st_offset_counter;
	int data_struct=0;
	int end=1;
	int dup_name_check=0;
	
	numPtr2=malloc(MAX_MEM);
	/* for DEBUG
	printf("256 byte of Memory allocated\n");
	printf("Memory address = %p\n", numPtr2);
	*/
	while(end){
		dup_name_check=0;
		printf("Do you want to allocate data(1) or deallocate data(2) ?\n");
		scanf("%d", &choice);
		while (getchar() != '\n');
		if(choice==1){
			printf("What is the type of data you want to allocate and what will be the name of the data?\n");
			scanf("%s", sec_choice);
			scanf("%s", box_name[count-1]);
			for(int i=0;i<count-1;i++){
				if(strcmp(box_name[count-1], box_name[i])==0){
					dup_name_check=1;
				}
			}
			if(dup_name_check!=1){
			//if you wrote int
			if(strcmp(sec_choice, choice2[0])==0){
				if(check_valid(box_offset[count-1], sizeof(int))!=-1){
				printf("Please specify a value for the data type\n");
				scanf("%d", &input_int);
				int_mem_allocator(numPtr2, box_offset[count-1], input_int);
				box_offset[count]=box_offset[count-1]+4;
				printf("%s has been allocated successfully\n", box_name[count-1]);
				}else{
					count--;
				}
			}
			
			//if you wrote double
			if(strcmp(sec_choice, choice2[1])==0){
				if(check_valid(box_offset[count-1], sizeof(double))!=-1){
				printf("Please specify a value for the data type\n");
				scanf("%lf", &input_double);
				double_mem_allocator(numPtr2, box_offset[count-1], input_double);
				box_offset[count]=box_offset[count-1]+8;
				printf("%s has been allocated successfully\n", box_name[count-1]);
				}else{
					count--;
				}
			}
			
			//if you wrote struct
			if(strcmp(sec_choice, choice2[2])==0){

				//st_offset_counter => struct의 offset을 계산하기 위해서 사용.
				st_offset_counter=0;
				printf("How many data should be in the struct\n");
				scanf("%d", &data_struct);
				printf("Please specify each type and its value\n");
				for(int i=0;i<data_struct;i++){
					scanf("%s", third_choice);
					//if you wrote int.
					if(strcmp(third_choice, choice2[0])==0){
						if(check_valid(box_offset[count-1], st_offset_counter+sizeof(int))!=-1){
						scanf("%d", &input_int);
						int_mem_allocator(numPtr2, box_offset[count-1]+st_offset_counter, input_int);
						st_offset_counter+=4;
						}else{
					count--;
				}
					}
					//if you wrote double
					if(strcmp(third_choice, choice2[1])==0){
						if(check_valid(box_offset[count-1], st_offset_counter+sizeof(double))!=-1){
						scanf("%lf", &input_double);
						double_mem_allocator(numPtr2, box_offset[count-1]+st_offset_counter, input_double);
						st_offset_counter+=8;
						}else{
					count--;
				}
					}
					//if you wrote char
					if(strcmp(third_choice, choice2[3])==0){
						if(check_valid(box_offset[count-1], st_offset_counter+sizeof(char))!=-1){
						scanf(" %c", &input_char);
						char_mem_allocator(numPtr2, box_offset[count-1]+st_offset_counter, input_char);
						st_offset_counter+=1;
						}else{
					count--;
				}
					}
				}
				box_offset[count]=box_offset[count-1]+st_offset_counter;
				printf("%s has been allocated successfully\n", box_name[count-1]);
			}	
			//if you wrote char
			if(strcmp(sec_choice, choice2[3])==0){
				if(check_valid(box_offset[count-1], sizeof(char))!=-1){
					printf("Please specify a value for the data type\n");
					scanf(" %c", &input_char);
					printf("input char: %c\n", input_char);
					char_mem_allocator(numPtr2, box_offset[count-1], input_char);
					box_offset[count]=box_offset[count-1]+1;
					printf("%s has been allocated successfully\n", box_name[count-1]);
				}else{
					count--;
				}
			}
			box_size[count-1]=box_offset[count]-box_offset[count-1];
			//후처리작업.
			count++;
			dump_mem(numPtr2, 256);
			
		}else{
			printf("Duplicated name entered. return to beginning...\n");
		}
		}
		else if(choice==2){
			if(count==1){printf("nothing to deallocate\n");}
			else{
				printf("What is the name of data you want to deallocate?\n");
				scanf("%s", sec_choice);
				for(int i=0;i<count-1;i++){
					// 만약 같으면. -> 배열 재정의.
					if(strcmp(sec_choice, box_name[i])==0){
						//origin size를 지정해서 나중에 써먹어야됨. 왜냐면 계산하면서 box_size는 변하기때문.
						int size_origin=box_size[i];
						//i번째에서 데이터 추출해서 깎아야지.
						clear_memory(numPtr2,box_offset[i], box_size[i]);
						pull_memory(numPtr2, box_offset[i], box_offset[count-1], box_size[i]);
						//dump_mem(numPtr2, 256);
						
						//DEBUG
						//printf("Count: %d box offset of last: %d, box size of last: %d\n", count, box_offset[count-2], box_size[count-2]);
						
						for(int j=i;j<count-1;j++){
							box_size[j]=box_size[j+1];
							box_offset[j]=box_offset[j+1]-size_origin;
							strcpy(box_name[j], box_name[j+1]);
						}
						box_offset[count-1]=0;
						box_size[count-1]=0;
						strcpy(box_name[count-1], "");
						rearr_memory(numPtr2, box_offset[count-2], size_origin);
					}
				}
			count--;
			dump_mem(numPtr2, 256);
			}
		}
		else{
			printf("Wrong input. go to first...\n");
		}
			/* for DEBUG
			//offset_shower for DEBUG
			printf("box_offset =");
			for (int i=0;i<count;i++) { printf("%d ", box_offset[i]); }
			printf("\n");
			//size_shower for DEBUG
			printf("box_size =");
			for (int i=0;i<count-1;i++){
				printf("%d ", box_size[i]);
			}
			printf("\n");
			//name_shower for DEBUG
			printf("box_name =");
			for (int i=0;i<count-1;i++) { printf("%s ", box_name[i]); }
			printf("\n");
			*/
	}
	
	free(numPtr2);
}




```

20170800.h
----------
```c
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
```

