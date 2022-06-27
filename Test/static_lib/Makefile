CC = gcc
OBJS = calculate.o error_handling.o
all : libkhm.a server client


libkhm.a : ${OBJS}
	ar rscv libkhm.a ${OBJS}
calculate.o :
	${CC} -g -c calculate.c
error_handling.o :
	${CC} -g -c error_handling.c


server :
	${CC} -g -o server main.c -L. -lkhm


client :
	${CC} -g -o client input.c


clean :
	rm -f *.o server libkhm.so client
