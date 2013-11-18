CFLAGS = -pedantic -Wall -g -Wextra -std=c99
CC=gcc-4.8
.PHONY = clean run all cleanall
EXE = main


###### HELP #####
# $< - prvy argument v dependency
# $@ - target
# $^ - vsetky dependencies
###############

all : main

scanner.o: scanner.c scanner.h types.h
	$(CC) $(CFLAGS) -o $@ -c $<

file_io.o : file_io.c file_io.h
	$(CC) $(CFLAGS) -o $@ -c $<
    
syntax.o: syntax.c types.h expressions.h syntax.h scanner.h
	$(CC) $(CFLAGS) -o $@ -c $<
	
expr.o: expressions.c types.h expressions.h scanner.h
	$(CC) $(CFLAGS) -o $@ -c $<
	
main.o : main.c types.h file_io.h
	$(CC) $(CFLAGS) -o $@ -c $<

test.o : test.c types.h file_io.h
	$(CC) $(CFLAGS) -o $@ -c $<
	
main : main.o file_io.o scanner.o syntax.o expr.o
	$(CC) $(CFLAGS) -o $@ $^
    
test : test.o file_io.o scanner.o
	$(CC) $(CFLAGS) -o $@ $^
	
clean:
	$(RM) *.o

cleanall :
	$(RM) *.o
	$(RM) $(EXE)