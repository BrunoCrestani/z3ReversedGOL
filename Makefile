CC = g++
CFLAGS = -Wall -std=c++17
Z3 = -lz3

objects = main.o utils.o

# Default target
reverseGameOfLife: $(objects)
	$(CC) -o reverseGameOfLife $(objects) $(Z3)

# Target for enabling verification (`-DVERIFY`)
verify: CFLAGS += -DVERIFY
verify: $(objects)
	$(CC) $(CFLAGS) -o reverseGameOfLife $(objects) $(Z3)

utils.o: utils.cpp
	$(CC) $(CFLAGS) -c utils.cpp

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

clean:
	rm -f $(objects) reverseGameOfLife