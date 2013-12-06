CFLAGS = -pedantic -Wall -Wextra -std=c99 -O2
CC=gcc-4.8
.PHONY = clean cleanall debug cov prof
EXE = main
OBJ = main.o file_io.o scanner.o syntax.o expressions.o ial.o built-in.o generator.o debug.o runtime.o

###### HELP #####
# $< - prvy argument v dependency
# $@ - target
# $^ - vsetky dependencies
###############
main : $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

    
debug : CFLAGS = -pedantic -Wall -Wextra -std=c99 -g3
debug : cleanall $(OBJ)
	$(CC) $(CFLAGS) -o main-debug $(OBJ)
    
prof : CFLAGS = -pedantic -Wall -Wextra -std=c99 -g3 -pg
prof : cleanall $(OBJ)
	$(CC) $(CFLAGS) -o main-prof $(OBJ)
    
cov : CFLAGS = -pedantic -Wall -Wextra -std=c99 -fprofile-arcs -ftest-coverage -g3
cov : cleanall $(OBJ)
	$(CC) $(CFLAGS) -o main-cov $(OBJ)
    
-include Makefile.deps

Makefile.deps : *.c *.h
	$(CC) $(CFLAGS) -MM *.c > Makefile.deps
  
%.o: %.c
	gcc -c $(CFLAGS) $*.c -o $*.o
    
## clean pravidla
clean:
	$(RM) *.o
	$(RM) Makefile.deps

cleanall :
	$(RM) *.o
	$(RM) Makefile.deps
	$(RM) $(EXE) gentest test-built-in main lextest main-debug main-prof main-cov
    
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
