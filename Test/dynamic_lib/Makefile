CC = gcc
OBJS = calculate.o error_handling.o
SRC = calculate.c error_handling.c
all : libkhm.so server client


libkhm.so :
	${CC} -g -fPIC -c ${SRC}
	${CC} -shared -o libkhm.so ${OBJS}


server :
	${CC} -g -o server main.c -L. -lkhm


client :
	${CC} -g -o client input.c


clean :
	rm -f *.o server libkhm.so client
