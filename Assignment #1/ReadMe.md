# Assignment 1 - Read Memory

1. 0과 1로 이루어진 input파일을 읽는다 (같은 디렉토리 내에서 input 이라는 파일이 있어야함)
2. 0과 1을 비트별로 읽은 후에, 다양한 data type을 이용해 출력을 한다.
3. input file의 사이즈는 클 수 있다.

출력해야할 파일은 다음과 같다.
>1. signed char
>2. ASCII codes
>3. unsigned char
>4. signed int
>5. unsigned int
>6. signed float
>7. unsigned float
>8. signed double
>9. unsigned double

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int calc(char* str);
void dump_mem_char(const void *mem, size_t len);
void dump_mem_asc(const void *mem, size_t len);
void dump_mem_uchar(const void *mem, size_t len);
void dump_mem_int(void *mem, size_t len);
void dump_mem_uint(void *mem, size_t len);
void dump_mem_float(void *mem, size_t len);
void dump_mem_double(void *mem, size_t len);
void swap_for_int(void *mem, int len);
void swap_for_double(void *mem, int len);

int main(){
        FILE *fp=fopen("input", "r");
        char buf[33];
        char tmp[33];
        int fileSize, size_16num;
        fseek(fp, 0, SEEK_END);
        fileSize = ftell(fp);
        size_16num=fileSize/32;
        //printf("글자수: %d, fileSize: %d\n", fileSize, size_16num);
        memset(buf, 0, sizeof(buf));
        memset(tmp, 0, sizeof(tmp));
        int i=0;
        int *inputData;
        inputData = calloc(size_16num, sizeof(int));

        //뒤에서부터 32자리씩 끊어서 읽어드린 후 print하는 함수
        for(int i=0;i<size_16num;i++){

                fseek(fp, 32*(i+1)-8, SEEK_SET);
                //뒤에서부터 끊어서 읽게 하기 위한 알고리즘 (Little Endian)
		for(int j=0;j<4;j++){
                        fread(tmp, 8, 1, fp);
                        fseek(fp, -16, SEEK_CUR);
                        strcat(buf, tmp);
                }
                // printf("%s, %ld", buf, ftell(fp)); <- 읽는 도중 디버깅 할때 사용. 파일 커서의 현위치 표시.

		// 파일을 읽어서 메모리에 쓰는 작업.
                *(inputData+i)=calc(buf);
                //printf("계산중.. 메모리 주소: %p\n", inputData+i);
        }
	
	/* 테스트용
	char *temp=(char *)inputData;
        printf("ptr1 = %p, 값: %X\n", temp, *temp);
        printf("ptr2 = %p, 값: %X\n", (temp+1), *(temp+1));
	printf("ptr2 = %p, 값: %X\n", (temp+2), *(temp+2));
	printf("ptr2 = %p, 값: %X\n", (temp+3), *(temp+3));
	*/

	
        dump_mem_char(inputData, fileSize/8); //char형 표시하게 하는 함수 (16진수 2자리)
	dump_mem_asc(inputData, fileSize/8); //ASCII를 표시하게 하는 함수
        dump_mem_uchar(inputData, fileSize/8); //unsigned char형 표시하게 하는 함수(16진수 2자리)
	dump_mem_int(inputData, fileSize/32); //int형 표시하게 하는 함수
	dump_mem_uint(inputData, fileSize/32); //unsigned int형 표시하게 하는 함수.
	dump_mem_float(inputData, fileSize/32); //float형 표시하게 하는 함수.
	dump_mem_double(inputData, fileSize/64); //double형 표시하게 하는 함수.

	free(inputData); //메모리 동적할당 해제
        fclose(fp); //파일 입력 중단. 함수 끝.
        return 0;
}

//파일 안에 있는 이진수를 읽어서 정수로 변환하는 함수.
int calc(char* str){
        int num=0, i;
        for(i=0;str[i]!=0;i++){
                num= ( num << 1 ) + str[i]- '0';
        }
        memset(str, 0, sizeof(str));
        return num;
}

//char형 표시하게 하는 함수 (16진수 2자리)
void dump_mem_char(const void *mem, size_t len){
        const char *buffer = mem;
        size_t i;
	printf("1. Char형: \n");
        for(i=0;i<len;i++){
                
                if(i>0 && i%8 == 0) {printf("\n"); }
		
                printf("%d ",(char)buffer[i]);
        }
        if(i>1 && i% 8 != 1) {puts("");}
}

//ASCII를 표시하게 하는 함수
void dump_mem_asc(const void *mem, size_t len){
       const char *buffer = mem;
       size_t i;
       printf("2. ASCII형: \n");
       for(i=0;i<len;i++){
		if(i>0 && i%8 == 0) {printf("\n");}
                if((buffer[i]&0xff)>128){
			printf(". ");
		}
		else{
		printf("%c ", (buffer[i] & 0xff));
		}
       }
       if(i>1 && i% 8 != 1) {puts("");}
}

//unsigned char형 표시하게 하는 함수(16진수 2자리)
void dump_mem_uchar(const void *mem, size_t len){
	const unsigned char *buffer = mem;
	size_t i;
	printf("3. Unsigned Char형:\n");
	for(i=0;i<len;i++){
                if(i>0 && i%8 == 0) {printf("\n");}
                printf("%d ", (unsigned char)(buffer[i] & 0xff));
       }
       if(i>1 && i% 8 != 1) {puts("");}
}

//int형 표시하게 하는 함수
void dump_mem_int(void *mem, size_t len){
        const int *buffer = mem;
	const char *tmp = mem;
	
        size_t i;
	printf("4. Int형:\n");

        for(i=0;i<len;i++){
		swap_for_int(mem, len);
                if(i>0 && i%8 ==0) {printf("\n");}
                printf("%d ", (int)(buffer[i] & 0xffffffff));
		swap_for_int(mem, len);
       }
       printf("\n");
}

//unsigned int형 표시하게 하는 함수.
void dump_mem_uint(void *mem, size_t len){
	const unsigned int *buffer = mem;
	size_t i;
	printf("5. Unsigned Int형: \n");

	for(i=0;i<len;i++){
		swap_for_int(mem, len);
		if(i>0 && i%8 ==0) {printf("\n");}
                printf("%u ", (unsigned int)(buffer[i] & 0xffffffff));
		swap_for_int(mem, len);
       }
       if(i>1 && i% 8 != 1) {puts("");}
}

//float형 표시하게 하는 함수
void dump_mem_float(void *mem, size_t len){
        const float *buffer = mem;
        size_t i;
	printf("6. Float형: \n");

        for(i=0;i<len;i++){
		swap_for_int(mem, len);
                if(i>0 && i%8 ==0) {printf("\n");}
                printf("%.4f ", (float)buffer[i]);
		swap_for_int(mem, len);
       }
       if(i>1 && i% 8 != 1) {puts("");}
}

//double형 표시하게 하는 함수.
void dump_mem_double(void *mem, size_t len){
        const double *buffer = mem;
        size_t i;
	printf("7. Double형:\n");

        for(i=0;i<len;i++){
		swap_for_double(mem, len);
                if(i>0 && i%8 ==0) {printf("\n");}
                printf("%.4f ", (double)buffer[i]);
		swap_for_double(mem, len);
       }
       if(i>1 && i% 8 != 1) {puts("");}
}

//4바이트 짜리를 읽어서 little endian을 제대로 읽게 하기 위한 swap함수.
void swap_for_int(void *mem, int len){
	char temp;
	char *buffer = mem;
	for(int i=0;i<len;i++)
	{
		for(int j=0;j<2;j++){
			temp = buffer[4*i+j];
			buffer[4*i+j] = buffer[4*(i+1)-j-1];
			buffer[4*(i+1)-j-1] = temp;
		}
	}
}

//8바이트 자료형에서 little endian을 제대로 읽게 하기 위한 swap함수.
void swap_for_double(void *mem, int len){
        char temp;
        char *buffer = mem;
        for(int i=0;i<len;i++)
        {
		for(int j=0;j<4;j++){
			temp = buffer[8*i+j];
                        buffer[8*i+j] = buffer[8*(i+1)-j-1];
                        buffer[8*(i+1)-j-1] = temp;

		}
        }
}


```
