CC = g++ 
CONSERVATIVE_FLAGS = -std=c++11 -Wall -Wextra -pedantic
DEBUGGING_FLAGS = -g -O0
CFLAGS = $(CONSERVATIVE_FLAGS) $(DEBUGGING_FLAGS)
csim: csim.o helper.o
		$(CC) -o csim csim.o helper.o

# Compiles hw5.cpp into an object file
csim.o: csim.cpp helper.h
	$(CC) $(CFLAGS) -c csim.cpp 

# Compile helper functions
helper.o: helper.cpp helper.h
	$(CC) $(CFLAGS) -c helper.cpp 
#
# 	# 'make clean' will remove intermediate & executable files
clean:
	rm -f *.o csim
