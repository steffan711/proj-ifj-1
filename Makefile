CFLAGS = -pedantic -Wall -Wextra -std=c99

.PHONY = clean run all cleanall
EXE = prototyp


###### HELP #####
# $< - prvy argument v dependency
# $@ - target
# $^ - vsetky dependencies
###############

all : prototyp file_io.o prototyp.o

file_io.o : file_io.c file_io.h
	$(CC) $(CFLAGS) -o $@ -c $<
	
prototyp.o : prototyp.c types.h file_io.h
	$(CC) $(CFLAGS) -o $@ -c $<
	
prototyp : prototyp.o file_io.o
	$(CC) $(CFLAGS) -o $@ $^
	
clean:
	$(RM) *.o

cleanall :
	$(RM) *.o
	$(RM) $(EXE)