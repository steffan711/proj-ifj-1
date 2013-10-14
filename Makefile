CFLAGS = -pedantic -Wall -Wextra -std=c99

.PHONY = clean run all cleanall
EXE = main


###### HELP #####
# $< - prvy argument v dependency
# $@ - target
# $^ - vsetky dependencies
###############

all : main file_io.o main.o

file_io.o : file_io.c file_io.h
	$(CC) $(CFLAGS) -o $@ -c $<
	
main.o : main.c types.h file_io.h
	$(CC) $(CFLAGS) -o $@ -c $<
	
main : main.o file_io.o
	$(CC) $(CFLAGS) -o $@ $^
	
clean:
	$(RM) *.o

cleanall :
	$(RM) *.o
	$(RM) $(EXE)