
PROJECT_NAME:= main
CC := /home/olav/buildroot-2021.08.2/output/host/bin/arm-buildroot-linux-uclibcgnueabihf-g++
INCLUDE := -I/home/olav/buildroot-2021.08.2/output/host/arm-buildroot-linux-uclibcgnueabihf/include/c++/10.3.0/ -L/home/olav/buildroot-2021.08.2/output/host/arm-buildroot-linux-uclibcgnueabihf/lib/
LIBS := -lcurl 
 
CurlFolder 	:= CURL
SpiFolder  	:= SPI
BackupFolder:= BACKUP
ListFolder	:= LIST
libFolder 	:= binaries

spi 		:= $(libFolder)/spi.o
curl 		:= $(libFolder)/curl.o
backup		:= $(libFolder)/backup.o
list		:= $(libFolder)/list.o
main		:= $(libFolder)/main.o

libName		:= $(libFolder)/connectBelt.a

.PHONY: lib
lib: $(libName)
.PHONY: build
build: $(PROJECT_NAME)

$(PROJECT_NAME): $(main) $(libName)
	$(CC) $(C_FLAGS)  $^ -o $@  $(INCLUDE) $(LIBS)

$(libName): $(spi) $(curl) $(backup) $(list)
	ar rcs $@ $^


$(main): main.cpp
	$(CC) $(C_FLAGS) -c  $< -o $@ $(INCLUDE) $(LIBS) 

$(spi): $(SpiFolder)/*.c $(SpiFolder)/*.h
	$(CC) $(C_FLAGS) -c -o$@  $<  $(INCLUDE)

$(curl): $(CurlFolder)/*.c $(SpiFolder)/*.h
	$(CC) $(C_FLAGS) -c -o$@  $<  $(INCLUDE) $(LIBS) 

$(backup): $(BackupFolder)/*.c $(BackupFolder)/*.h
	$(CC) $(C_FLAGS) -c -o$@  $<  $(INCLUDE)

$(list): $(ListFolder)/*.c $(ListFolder)/*.h
	$(CC) $(C_FLAGS) -c -o$@  $<  $(INCLUDE)
 
.PHONY: clean
clean: 
	rm -f  $(libFolder)/* $(PROJECT_NAME) 