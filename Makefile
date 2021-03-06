GCOV_OUTPUT = *.gcda *.gcno *.gcov 
GCOV_CCFLAGS = -fprofile-arcs -ftest-coverage
CC     = gcc
CCFLAGS = -I. -Itests -g -O2 -Wall -Werror -fno-omit-frame-pointer -fno-common -fsigned-char $(GCOV_CCFLAGS)

all: test

main.c:
	sh tests/make-tests.sh tests/test_*.c > main.c

test: main.c chunkybar.o tests/test_chunkybar.c tests/CuTest.c main.c
	$(CC) $(CCFLAGS) -o $@ $^
	./test
	gcov main.c chunkbar.c

chunkybar.o: chunkybar.c
	$(CC) $(CCFLAGS) -c -o $@ $^

clean:
	rm -f main.c chunkybar.o test $(GCOV_OUTPUT)
