TARGET = median
CC = gcc
CFLAGS = -o2 -Wall

.PHONY: all clean

all: $(TARGET)

$(TARGET): median.o medianAlgo.o
	$(CC) median.o medianAlgo.o -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf *.o $(TARGET)

