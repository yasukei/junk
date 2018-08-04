TARGET=bin/gcc-5.4.0/debug/main

all: debug

clean:
	bjam --clean

debug:
	time bjam -j4 variant=debug
	#time bjam -j4 -d+2 variant=debug

release:
	time bjam -j4 variant=release
	#time bjam -j4 -d+2 variant=release

rebuild: clean all

run: all
	./$(TARGET)

