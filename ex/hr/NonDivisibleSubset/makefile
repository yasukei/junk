TARGET=main

CPPFLAGS=-pg -O0 -g

$(TARGET): main.cpp

build: $(TARGET)

run: $(TARGET)
	./$(TARGET) < input.txt
	gprof ./$(TARGET) gmon.out | head -n 20

debug: $(TARGET)
	gdb ./$(TARGET)

clean:
	rm -f ./$(TARGET)

rebuild: clean build


