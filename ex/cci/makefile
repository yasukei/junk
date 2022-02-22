TARGET=main
CPPFLAGS=-g -O0

$(TARGET): main.cpp

rebuild: clean build

clean:
	rm -f $(TARGET)

build: $(TARGET)

run: build
	./$(TARGET)
