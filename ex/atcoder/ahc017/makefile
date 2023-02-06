
TARGET=main

CXXFLAGS=-g -O0 -Wall
CXXFLAGS=-pg -O3 -Wall
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

measure: $(TARGET)
	echo -n "500,  10, "; ./$(TARGET) < in/0064.txt | head -n 1
	echo -n "500,  20, "; ./$(TARGET) < in/0031.txt | head -n 1
	echo -n "600,  10, "; ./$(TARGET) < in/0022.txt | head -n 1
	echo -n "600,  20, "; ./$(TARGET) < in/0027.txt | head -n 1
	echo -n "600,  30, "; ./$(TARGET) < in/0086.txt | head -n 1
	echo -n "700,  10, "; ./$(TARGET) < in/0069.txt | head -n 1
	echo -n "700,  20, "; ./$(TARGET) < in/0078.txt | head -n 1
	echo -n "700,  30, "; ./$(TARGET) < in/0068.txt | head -n 1
	echo -n "800,  10, "; ./$(TARGET) < in/0000.txt | head -n 1
	echo -n "800,  20, "; ./$(TARGET) < in/0049.txt | head -n 1
	echo -n "800,  30, "; ./$(TARGET) < in/0058.txt | head -n 1
	echo -n "900,  10, "; ./$(TARGET) < in/0040.txt | head -n 1
	echo -n "900,  20, "; ./$(TARGET) < in/0075.txt | head -n 1
	echo -n "900,  30, "; ./$(TARGET) < in/0099.txt | head -n 1
	echo -n "1000, 20, "; ./$(TARGET) < in/0047.txt | head -n 1

$(TARGET):

clean:
	rm -f $(TARGET)

rebuild: clean all

