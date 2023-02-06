
TARGET=main

#CXXFLAGS=-g -O0 -Wall
#CXXFLAGS=-pg -O3 -Wall
CXXFLAGS=-O3 -Wall

all: $(TARGET)

run: $(TARGET)
	./$(TARGET) < debug_input.txt | tee debug_output.txt

debug: $(TARGET)
	gdb ./$(TARGET)

prof: $(TARGET)
	./$(TARGET) < debug_input.txt; gprof ./$(TARGET) gmon.out > gmon.txt

$(TARGET):

clean:
	rm -f $(TARGET)

rebuild: clean all

