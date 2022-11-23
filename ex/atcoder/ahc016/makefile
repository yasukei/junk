#TARGET=main
#
#CPPFLAGS=-g -Wall -O0
##CPPFLAGS=-Wall -O1 -pg
##CPPFLAGS=-Wall -O3
#
#$(TARGET): main.cpp
#
#build: $(TARGET)
#
#run: $(TARGET)
#	./$(TARGET) < input.txt
#
#prof: run
#	gprof ./$(TARGET) gmon.out | less
#
#clean:
#	rm -f ./$(TARGET)
#
#rebuild: clean build

TARGET=main.py

all:

run: all
	cd tools && ~/.cargo/bin/cargo run --release --bin tester python3 ../main.py < ../input.txt
	#python3 ./$(TARGET) xxx
