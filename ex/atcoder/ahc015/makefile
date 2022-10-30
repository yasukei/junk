TARGET=main

#CPPFLAGS=-g -Wall -O0
#CPPFLAGS=-Wall -O3 -pg
CPPFLAGS=-Wall -O3

$(TARGET): main.cpp

build: $(TARGET)

run: $(TARGET)
	./$(TARGET) < input.txt

prof: run
	gprof ./$(TARGET) gmon.out | less

clean:
	rm -f ./$(TARGET)

rebuild: clean build

