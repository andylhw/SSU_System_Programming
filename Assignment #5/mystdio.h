#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BUFSIZE 1024
#define EOF -1
#define stdin 1
#define stdout 2
#define stderr 3
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

typedef struct myFile{
	int fd;
	int post;
	int size;
	int actual_size;
	char *mode;
	int flag;
	char lastop;
	int eof;
	char *buffer;
}FILE;

int fwrite(void *ptr, int size, int nmemb, FILE *stream);
int getFileSize(FILE *stream);

//for DEBUG
/*
void show_FILE(FILE *fp){
	printf("FILES STATUS: \n");
	printf("fd: %d\n", fp->fd);
	printf("post: %d\n", fp->post);
	printf("size: %d\n", fp->size);
	printf("actual size: %d\n", fp->actual_size);
	printf("mode: %s\n", fp->mode);
	printf("flag: %d\n", fp->flag);
	printf("eof: %d\n", fp->eof);
	printf("buffer: %s\n\n\n", fp->buffer);
}
*/

FILE *fopen(const char *pathname, const char *mode){

	FILE temp;
	FILE *fp = &temp;
	memset(fp, 0, sizeof(struct myFile)); 
	//show_FILE(fp);
	int fd;
	char buffer[4096];
	int errCode = -1;
	if(mode == "r"){
		fp->flag = O_RDONLY;
		fd = open(pathname, O_RDONLY);
		errCode = fd;
	}
	if(mode == "r+"){
		fp->flag = O_RDWR;
		fd = open(pathname, O_RDWR, 0666);
		errCode = fd;
	}
	if(mode == "rw"){
		fp->flag = O_RDWR | O_TRUNC;
		fd = open(pathname, O_RDWR|O_TRUNC, 0666);
		errCode = fd;
	}
	if(mode == "w"){
		fp->flag = O_WRONLY | O_CREAT | O_TRUNC;
		fd = open(pathname, O_WRONLY| O_CREAT |O_TRUNC, 0666);
		errCode = fd;
	}
	if(mode =="w+"){
		fp->flag = O_RDWR|O_CREAT|O_TRUNC;
		fd = open(pathname, O_RDWR|O_CREAT|O_TRUNC, 0666);
		errCode = fd;
	}
	if(mode == "a"){
		fp->flag = O_WRONLY|O_APPEND|O_CREAT;
		fd = open(pathname, O_WRONLY|O_APPEND|O_CREAT, 0666);
		errCode = fd;
	}
	if(mode == "a+"){
		fp->flag = O_RDWR|O_APPEND|O_CREAT;
		fd = open(pathname, O_RDWR|O_APPEND|O_CREAT, 0666);
		errCode = fd;
	}
	if(errCode !=-1){
		fp->fd = fd;
		//printf("%d\n", fp->fd);
	}
	else{
		//printf("Error!.");
		return NULL;
	}
	fp->mode = (char*)mode;
	//printf("File Opened. fd: %d, mode: %s\n", fp->fd, fp->mode);
	return fp;
}

int fread(void *ptr, int size, int nmemb, FILE *stream){
	int result;
	int bytes = 0;
	char *p = ptr;
	int count = size * nmemb;
	char buffer[BUFSIZE];
	//printf("fread init. count = %d, fd = %d\n", count, stream->fd);
	while(count){
		result = read(stream->fd, p, count);
		if(result == -1){
			if(errno ==EINTR || errno == EAGAIN) {continue;}
			else {break;}
		}
		else if(result ==0){
			break;
		}
		
		p+= result;
		bytes += result;
		count -= result;
	}
	stream->actual_size = getFileSize(stream);
	if(size!=1&&nmemb!=1){
	if (read(stream->fd, buffer, stream->actual_size)!=0){
		stream->buffer = buffer;
	}
	}
	return bytes;
}
int fwrite(void *ptr, int size, int nmemb, FILE *stream){
	int result;
	int count = size*nmemb;
	const char* p = ptr;
	char buffer[BUFSIZE];
	if(stream->flag & O_RDONLY == 0){
		return 0;
	}
	//printf("fwrite - File Descripter: %d\n", stream->fd);
	result = write(stream->fd, p, count);
	stream->size += size*nmemb;
	if(result == -1){
		if(errno == EINTR || errno == EAGAIN) {return 0;}
		else {return 0;}
	}
	else if(result == 0){
		return result;
	}
	
	p+= result;
	int bytes =0;
	bytes+=result;
	count -= result;
	
	//stream의 buffer를 가져온 값으로 채운다.
	stream->buffer = (char*)ptr;
	//DEBUG
	stream->actual_size = getFileSize(stream);
	//show_FILE(stream);
	//printf("read in fwrite init\n");
	if (read(stream->fd, buffer, stream->actual_size)!=0){
		
		stream->buffer = buffer;
	}
	
	//printf("%s\n", buffer);
	return nmemb;
}
int fflush(FILE *stream){
	stream->buffer = "";
	return 0;
}
int fclose(FILE *stream){
	int result;
	result = close(stream->fd);
	return result;
}

int fseek(FILE *stream, int offset, int whence){
	int result;
	result = (int)lseek(stream->fd, offset, whence);
	stream->size = result;
	if(stream->size > getFileSize(stream)){
		stream->eof = 1;
	}
	if(result == -1) //Error
	{ 
		return -1; 
	}
	else{
		return 0;
	}
}
int fgetc(FILE *stream){
	unsigned char  ch;
	int result = fread(&ch, 1, 1, stream);
	stream->actual_size = getFileSize(stream);
	if(result == 1)
	{
		return (int)ch;
	}
	else{
		stream->eof=1;
		return EOF;
	};
	
}
int fputc(int c, FILE *stream){
	unsigned char ch = c;
	stream->actual_size = getFileSize(stream);
	int result = fwrite(&ch, 1,1, stream);
	if(result == 1){ return ch;}
	else {return EOF;};
}

//파일사이즈 읽는 메소드. 따로 추가해서 actualsize구할때 사용하려함.
int getFileSize(FILE *stream){
	struct stat sb;
	if(fstat(stream->fd, &sb) == -1){
		return 1;
	}
	return sb.st_size;
}

int feof(FILE *stream){
	if(stream->eof == 1){
		return -1;
	}
	else{
		return 1;	
	}
}
