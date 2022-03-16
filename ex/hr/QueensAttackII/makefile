TARGET=main

#CPPFLAGS=-pg -O0 -g
CPPFLAGS=-pg -O0
#CPPFLAGS=-pg -O3 -pthread

$(TARGET): main.cpp

build: $(TARGET)

run: $(TARGET)
	time ./$(TARGET) < input.txt
	gprof ./$(TARGET) gmon.out | head -n 20

debug: $(TARGET)
	gdb ./$(TARGET)

clean:
	rm -f ./$(TARGET)

rebuild: clean build


