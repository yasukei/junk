TARGET=bin/gcc-5.4.0/debug/main

all:
	time bjam -j2

clean:
	bjam clean

rebuild: clean all

run: all
	./$(TARGET)
