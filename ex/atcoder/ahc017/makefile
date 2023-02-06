
TARGET=main

CXXFLAGS=-g -O0 -Wall
#CXXFLAGS=-pg -O3 -Wall
CXXFLAGS=-O3 -Wall

all: $(TARGET)

run: $(TARGET)
	./$(TARGET) < in/0000.txt | tee debug_output.txt

run_1000: $(TARGET)
	./$(TARGET) < in/0047.txt | tee debug_output.txt

run_900: $(TARGET)
	./$(TARGET) < in/0088.txt | tee debug_output.txt

run_800: $(TARGET)
	./$(TARGET) < in/0034.txt | tee debug_output.txt

run_700: $(TARGET)
	./$(TARGET) < in/0004.txt | tee debug_output.txt

run_600: $(TARGET)
	./$(TARGET) < in/0022.txt | tee debug_output.txt

run_500: $(TARGET)
	./$(TARGET) < in/0011.txt | tee debug_output.txt

debug: $(TARGET)
	gdb ./$(TARGET)

prof: $(TARGET)
	./$(TARGET) < debug_input.txt; gprof ./$(TARGET) gmon.out > gmon.txt

$(TARGET):

clean:
	rm -f $(TARGET)

rebuild: clean all

