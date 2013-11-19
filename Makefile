CFLAGS = -pedantic -Wall -Wextra -std=c99

.PHONY = clean run all cleanall
EXE = main


###### HELP #####
# $< - prvy argument v dependency
# $@ - target
# $^ - vsetky dependencies
###############

all: main

ial.o: ial.c ial.h
	$(CC) $(CFLAGS) -o $@ -c $<

built-in.o: built-in.c built-in.h types.h ial.h
	$(CC) $(CFLAGS) -o $@ -c $<

file_io.o: file_io.c file_io.h
	$(CC) $(CFLAGS) -o $@ -c $<

main.o: main.c types.h file_io.h
	$(CC) $(CFLAGS) -o $@ -c $<

main: main.o file_io.o built-in.o ial.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	$(RM) *.o

cleanall :
	$(RM) *.o
	$(RM) $(EXE)

clear_screen:
	clear

## \/ \/ \/ TESTY \/ \/ \/

tests: clear_screen test-built-in

test-built-in.o: test-built-in.c types.h
	$(CC) $(CFLAGS) -o $@ -c $<

test-built-in: built-in.o test-built-in.o ial.o
	$(CC) $(CFLAGS) -o $@ $^
