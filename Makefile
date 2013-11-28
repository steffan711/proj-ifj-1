CFLAGS = -pedantic -Wall -Wextra -g -std=c99
#CC=gcc-4.8
.PHONY = clean run all cleanall
EXE = main


###### HELP #####
# $< - prvy argument v dependency
# $@ - target
# $^ - vsetky dependencies
###############

all : main

ial.o: ial.c ial.h
	$(CC) $(CFLAGS) -o $@ -c $<
	
built-in.o: built-in.c built-in.h types.h ial.h generator.h scanner.h
	$(CC) $(CFLAGS) -o $@ -c $<

scanner.o: scanner.c scanner.h types.h
	$(CC) $(CFLAGS) -o $@ -c $<

file_io.o : file_io.c file_io.h
	$(CC) $(CFLAGS) -o $@ -c $<
    
syntax.o: syntax.c types.h expressions.h syntax.h scanner.h generator.h debug.h
	$(CC) $(CFLAGS) -o $@ -c $<
	
expr.o: expressions.c types.h expressions.h scanner.h generator.h debug.h
	$(CC) $(CFLAGS) -o $@ -c $<
    
generator.o : generator.c generator.h types.h
	$(CC) $(CFLAGS) -o $@ -c $<

runtime.o : runtime.c runtime.h types.h generator.h
	$(CC) $(CFLAGS) -o $@ -c $<
	
main.o : main.c types.h file_io.h
	$(CC) $(CFLAGS) -o $@ -c $<
    
debug.o : debug.c scanner.h debug.h
	$(CC) $(CFLAGS) -o $@ -c $<
	
main : main.o file_io.o scanner.o syntax.o expr.o ial.o built-in.o generator.o debug.o runtime.o
	$(CC) $(CFLAGS) -o $@ $^
clean:
	$(RM) *.o

cleanall :
	$(RM) *.o
	$(RM) $(EXE) gentest test
    
clear_screen:
	clear
	
## \/ \/ \/ TESTY \/ \/ \/

tests: clear_screen test-built-in lextest gentest

lextest.o : lextest.c types.h file_io.h debug.h
	$(CC) $(CFLAGS) -o $@ -c $<

gentest.o : gentest.c types.h generator.h debug.h
	$(CC) $(CFLAGS) -o $@ -c $<

test-built-in.o: test-built-in.c types.h scanner.h generator.h
	$(CC) $(CFLAGS) -o $@ -c $<

lextest : lextest.o file_io.o scanner.o debug.o
	$(CC) $(CFLAGS) -o $@ $^

gentest : gentest.o file_io.o scanner.o generator.o debug.o
	$(CC) $(CFLAGS) -o $@ $^
	
test-built-in: built-in.o test-built-in.o ial.o scanner.o
	$(CC) $(CFLAGS) -o $@ $^
