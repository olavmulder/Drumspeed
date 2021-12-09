CC := arm-linux-gnueabihf-gcc -std=gnu11
C_FLAGS := -Wconversion -Wextra -Wall

name := LinkedList.o
$(name): LIST\*.c LIST\*.h
	$(CC) $(C_FLAGS) -c -o$@  $<  $(LIBS)
