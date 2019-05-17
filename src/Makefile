CC=g++
CFLAGS=-std=c++0x -I.
LDFLAGS=-pthread
DEPS = connection.hpp ecc.hpp endecode.hpp
TARGET=CTC-lib

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LDFLAGS)

build: ecc.o endecode.o main.o
	$(CC) -o $(TARGET) $^ $(CFLAGS) $(LDFLAGS)

clean:
	rm ./$(TARGET) *.o
