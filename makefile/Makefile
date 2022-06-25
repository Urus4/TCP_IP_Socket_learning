OBJS = main.o print.o input.o
TARGET = test
cc = gcc
CFLAGS = -g
LDFLAGS = -lpthread


$(TARGET) : $(OBJS)
	$(cc) -o $@ $^ $(CFLAGS) 
.c.o:
	$(cc) -c -o $@ $<
clean :
	rm -f *.o $(TARGET)
