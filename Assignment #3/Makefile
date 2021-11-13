CC=gcc
CFLAGS= -Wall -g -I.

# Change the name of source code files 
# student ID #.h (e.g., 2021679.h)
# student ID #.c (e.g., 2021679.c)
DEPS = 20170800.h
OBJ = 20170800.o 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# Change the name of the output binary file to your student ID #.
20170800: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

clean:
	rm -rf *.o 20170800

