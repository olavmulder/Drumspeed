#/home/olav/buildroot-2021.08.2/output/host/bin/arm-buildroot-linux-uclibcgnueabihf-g++ -o 
#main C++/main.cpp BACKUP/backup.c CURL/curl.c LIST/linkedList.c SPI/spi.c -lcurl

PROJECT_NAME:= main
CC := /home/olav/buildroot-2021.08.2/output/host/bin/arm-buildroot-linux-uclibcgnueabihf-g++
C_FLAGS := -Wconversion -Wextra -Wall -Werror 
INCLUDE := -I/home/olav/buildroot-2021.08.2/output/host/arm-buildroot-linux-uclibcgnueabihf/include/c++/10.3.0/ -L/home/olav/buildroot-2021.08.2/output/host/arm-buildroot-linux-uclibcgnueabihf/lib/
LIBS := -lcurl 
 
CurlFolder 	:= CURL
SpiFolder  	:= SPI
#TimerFolder := TIMER
BackupFolder:= BACKUP
ListFolder	:= LIST
libFolder 	:= binaries

spi 		:= $(libFolder)/spi.o
curl 		:= $(libFolder)/curl.o
#timer		:= $(libFolder)/timer.o
backup		:= $(libFolder)/backup.o
list		:= $(libFolder)/list.o
main		:= $(libFolder)/main.o

libName		:= $(libFolder)/connectBelt.a

main: main.cpp CURL/*.c SPI/*.c BACKUP/*.c LIST/linkedList.c
	$(CC) $(C_FLAGS)  $^  -o $@ $(LIBS)

#.PHONY: all
#$(PROJECT_NAME): $(main) $(libName)0
#	$(CC) $(C_FLAGS)  $^ -o $@  $(INCLUDE) $(LIBS)
#.PHONY: main
#$(main): C++/main.cpp 
#	$(CC) $(C_FLAGS) -c  $< -o $@ $(INCLUDE) $(LIBS) 
#.PHONY: lib
#$(libName): $(spi) $(curl) $(timer) $(backup) $(list)
#	ar rcs $@ $^ 
#.PHONY: spi
#$(spi): $(SpiFolder)/*.c $(SpiFolder)/*.h
#	$(CC) $(C_FLAGS) -c -o$@  $<  $(INCLUDE)
#.PHONY: curl
#$(curl): $(CurlFolder)/*.c $(SpiFolder)/*.h
#	$(CC) $(C_FLAGS) -c -o$@  $<  $(INCLUDE) $(LIBS) 
#.PHONY: timer
#$(timer): $(TimerFolder)/*.c $(TimerFolder)/*.h
#	$(CC) $(C_FLAGS) -c -o$@  $<  
#.PHONY: backup
#$(backup): $(BackupFolder)/*.c $(BackupFolder)/*.h
#	$(CC) $(C_FLAGS) -c -o$@  $<  $(INCLUDE)
#.PHONY: list 
#$(list): $(ListFolder)/*.c $(ListFolder)/*.h
#	$(CC) $(C_FLAGS) -c -o$@  $<  $(INCLUDE)
.PHONY: clean
clean: 
	rm -f  $(libFolder)/* $(PROJECT_NAME) 