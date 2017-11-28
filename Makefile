CC=gcc
CFLAGS=-Wall -Werror -pthread

main: main.o
	$(CC) ${CFLAGS} -o main main.o

run: main
./main
