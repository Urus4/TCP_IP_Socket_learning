OBJS = main.o print.o input.o
TARGET = test
cc = gcc
CFLAGS = -g
LDFLAGS = -lkhm

$(TARGET) : $(OBJS)
	$(cc) -o $@ $^ $(CFLAGS) $(FLAGS)
.c.o:
	$(cc) -c -o $@ $<
clean :
	rm -f *.o $(TARGET)
