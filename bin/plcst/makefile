TARGET=bin/gcc-5.4.0/debug/main

all:
	bjam

clean:
	bjam clean

rebuild: clean all

run: all
	./$(TARGET)
