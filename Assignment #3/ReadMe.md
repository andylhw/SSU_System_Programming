Assignment 3 - A basic multi-process program
=============

5줄 이상인 데이터를 읽는 파일을 만들어라.


조건
>0. 프로세스의 갯수는 5개로 만든다.
>1. 1개의 프로세스는 1줄을 읽은 다음, 다음 프로세스로 넘겨준다.
>2. 이런식으로 5번째 프로세스까지 이동한 후, 5번쨰 프로세스는 첫번째 프로세스로 넘겨준다
>3. 만약 파일을 다 읽으면, 5번 프로세스까지 다 읽었다는 신호를 보낸다.
>4. 5번 프로세스가 다 읽었다는 신호를 받으면, 4번에게 Exit신호를 주고, 자신은 Exit한다.
>5. Exit신호를 받은 프로세스는 앞 프로세스한테 Exit신호를 주고, 자신은 Exit한다.
>6. 1번 프로세스가 Exit신호를 받으면, Exit하면서 프로그램은 종료한다.

소스코드
============
20170800.c
--------------
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <20170800.h>

#define PROCESS_NUM 5
#define TARGET_NUM 9987123


int main(int argc, char* argv[]) {
    int pids[PROCESS_NUM];
    int pipes[PROCESS_NUM + 1][2];
    int pipes_back[PROCESS_NUM + 1][2];
    int i;	
    int status;
    
    
    if (argc != 2 ){
    	printf("Usage: 20170800 <file>\n");
	return 0;
    }
    FILE *fp = fopen(argv[1],"r");
    if(fp==NULL){
    	printf("Error: Unable to Open %s\n", argv[1]);
    }
    //fd = open(argv[1], O_RDWR|O_SYNC);
    /*
    if(fd<0) {
	printf("Error %d Unable to open %s\n", fd, argv[1]);
	return 0;
    }
    */
    
    //pipe생성하기. (pipe & pipe_back)
    for (i = 0; i < PROCESS_NUM + 1; i++) {
    	create_pipe(pipes[i]);
    	create_pipe(pipes_back[i]);
    }
    //Process 5개 생성하기.
    for (i = 0; i < PROCESS_NUM; i++) {
        pids[i] = fork();
        if (pids[i] == -1) {
            printf("Error with creating process\n");
            return 2;
        }
        if (pids[i] == 0) {
            // Child process
            int j;
            char str[1024];
            
            /*
            FILE *fp = fopen(argv[1],"r");
            if(fp==NULL){
            	printf("Error: Unable to Open %s\n", argv[1]);
            }
            */
            //쓰지 않는 pipe & pipe_back 닫기.
            for (j = 0; j < PROCESS_NUM + 1; j++) {
                if (i != j) {    
                	close(pipes[j][0]);
			close(pipes_back[j][0]);		
                }
                if (i + 1 != j) {
                    close(pipes[j][1]);
                }
            }
            //현재 x는 읽는 값. end_mode는 끝났음을 앞 프로세서한테 알리는 용도.
            int x=0;
            
            int end_mode=0;
            //while 루프를 통해서 프로세스가 계속 기다림.
            while(end_mode==0){
            	//pipe를 읽는다 (fd를 가져온담에, fseek를 활용해서 file을 찾아가고싶음.
            	if (read(pipes[i][0], &x, sizeof(int)) == -1) {
                	printf("Error at reading1\n");
                	return 3;
            	}
            	//목표에 도착했을때 (feof?)
            	else if(x==TARGET_NUM){
            		
            		//가진 숫자를 알려줌. 일단 나중에 수정 예정.
            		//뒤한테 현재값을 보내면, 뒤에서는 같은 작업 수행 예정.
            		if (write(pipes[i + 1][1], &x, sizeof(int)) == -1) {
                		printf("Error at writing1\n");
                		return 4;
            		}
            		//printf("(%d) Sent %d\n", getpid(), x);
            		//만약 i=4 ->끝이면.
            		if(i==4){
            			//end_mode를 999로 바꾼 다음, 앞에 파이프로 end_mode값 전송.
            			end_mode = 999;
                		//printf("Process(%d): I'm exiting...\n", getpid());
            			if (write(pipes_back[i - 1][1], &end_mode, sizeof(int)) == -1) {
                			printf("Error at writing2\n");
                			return 4;
                		}
                		
                		sleep(1);
                		if (write(pipes[i + 1][1], &x, sizeof(int)) == -1) {
        	        			printf("Error at writing6\n");
        	        			return 4;
        	        	}
        	        	printf("Sent to Main\n"); 
                		//close(pipes_back[3][1]);
                		//전송 후 종료.
            			exit(0);
            		}
            		//i=1~3이면.
            		if(i<4&&i>0){
            			
            			//end_mode를 꾸준히 기다리고있음. 만약 값이 도착하게 된다면.
            			if (read(pipes_back[i][0], &end_mode, sizeof(int)) == -1) {
                			printf("Error at reading last draft\n");
                			return 3;
            			}
            			//printf("Process(%d): I got END_NUM!", getpid()); DEBUG
            			//도착했는데, 그 값이 999(끝났을때 호출되는 값이면), 앞에 파이프로 end_mode 값 전송.
            			if(end_mode == 999){
            				if (write(pipes_back[i - 1][1], &end_mode, sizeof(int)) == -1) {
                				printf("Error at writing3\n");
                				return 4;
                			}
                			//printf("x1 Sent complete end_mode from process %d\n", getpid()); DEBUG
                			sleep(1);
                			//printf("Process(%d): I'm exiting...\n", getpid());
            				return 0;
            			}
            		}
            		//마지막 i==0일때를 구분해준 이유: 앞으로 전송할 필요 없이 끝내버리면 되니까!
            		if(i==0){
            			printf("Hi");
            			if (read(pipes_back[i][0], &end_mode, sizeof(int)) == -1) {
                			printf("Error at reading last draft\n");
                			return 3;
            			}
            			if(end_mode == 999){
                			//printf("Process(%d): I'm exiting...\n", i);
            				return 0;
            			}	
            		}
            	}
            	
            	else if( x == -1){
            		//printf("Process(%d) has -1\n", getpid()); DEBUG
            		if (write(pipes[i + 1][1], &x, sizeof(int)) == -1) {
        	        	printf("Error at writing4\n");
        	        	return 4;
        	    	}
        	    	if (read(pipes_back[i][0], &end_mode, sizeof(int)) == -1) {
                			printf("Error at reading last draft\n");
                			return 3;
            		}
            		//printf("Process(%d): End_mode value = %d\n", getpid(), end_mode); DEBUG
            		if(end_mode == 999){
            			if(i==0){
            				//printf("Process(%d): I'm exiting...\n", getpid());
            				return 0;
            			}
            			else{
            				//sleep(1);
	            			if (write(pipes_back[i - 1][1], &end_mode, sizeof(int)) == -1) {
	                			printf("Error at writing5\n");
	                			return 4;
	                		}
	                		//printf("process(%d): Sent complete end_mode from \n", getpid()); DEBUG
	                		//printf("Process(%d): I'm exiting...\n", getpid());
	            			return 0;
	            		}
            		}
            	}
            	//값이 끝나지 않았을때는 계속 실행시킨다.
            	else{
	            	//해야할 작업 수행.
        	    	fseek( fp, x, SEEK_SET);
        	    	int temp = x;
        	    	fseek(fp, temp, SEEK_SET);	
        	    	printf( "%d %s", getpid(),fgets( str, 1024, fp));
        	    	x=ftell(fp);
        	    	if(fgets(str, 1024, fp)==NULL){
        	    		if(i==4){
        	    			//end_mode를 999로 바꾼 다음, 앞에 파이프로 end_mode값 전송.
	            			end_mode = 999;
	            			if (write(pipes_back[i - 1][1], &end_mode, sizeof(int)) == -1) {
	                			printf("Error at writing2\n");
	                			return 4;
	                		}
	                		x=TARGET_NUM;
	                		if (write(pipes[i + 1][1], &x, sizeof(int)) == -1) {
	        	        		printf("Error at writing6\n");
	        	        		return 4;
	        	        	}
	        	        	sleep(1);
        	    			printf("%d Read all data\n", getpid());
	                		//printf("Process(%d): I'm exiting...\n", getpid());
	        	        	//printf("Sent to Main2\n");
	                		//close(pipes_back[3][1]);
	                		//전송 후 종료.
	            			exit(0);
        	    		}
        	    		else{
        	    			printf("%d Read all data\n", getpid());
        	    			x=TARGET_NUM;
        	    		}
        	    		}
        	    	
        	    	if (write(pipes[i + 1][1], &x, sizeof(int)) == -1) {
        	        	printf("Error at writing6\n");
        	        	return 4;
        	    	}
	    	}
            }
            //와일루프가 끝나고, 마무리해주는 모습.
            close(pipes[i][0]);
            close(pipes[i + 1][1]);
        }
    }

    // Main process
    int j;
    for (j = 0; j < PROCESS_NUM + 1; j++) {
        if (j != PROCESS_NUM) {
            close(pipes[j][0]);
        }
        if (j != 0) {
            close(pipes[j][1]);
        }
    }
    //초기값 전송.
    int y = 0;
    if (write(pipes[0][1], &y, sizeof(int)) == -1) {
        printf("Error at writing7\n");
        return 4;
    }
    while(y<TARGET_NUM){
    	if (read(pipes[PROCESS_NUM][0], &y, sizeof(int)) == -1) {
       	 printf("Error at reading2\n");
       	 return 3;
  	}	
   	if(y!=TARGET_NUM){
    		if (write(pipes[0][1], &y, sizeof(int)) == -1) {
      	 		printf("Error at writing8\n");
        		return 4;
        	}
    	}
    	if(y==TARGET_NUM){
    	//printf("Main Got TARGET\n"); DEBUG
    	y=-1;
    	if (write(pipes[0][1], &y, sizeof(int)) == -1) {
      	 	printf("Error at writing - main\n");
        	return 4;
        }
        //printf("WRITE COMPLETE"); DEBUG
    	
    	close(pipes[0][1]);
    	close(pipes[PROCESS_NUM][0]);
    	break;
    	}
    
    }
    //y가 TARGET_NUM이면, i=0한테 -1을 보내버려.
    
    //마지막까지 기다리다가, 다 끝나버리면, 프로그램 정상 종료.
    for (i = PROCESS_NUM-1; i > -1; i--) {
        waitpid(pids[i], &status, 0);
        printf("%d I'm exiting...\n", pids[i]);
    }
    return 0;
}



```

20170800.h
----------
```c
#include <stdio.h>
#include <unistd.h>


void create_pipe(int* fd){
	
	if (pipe(fd) == -1) {
            printf("Error with creating pipe\n");
        }
}




```

