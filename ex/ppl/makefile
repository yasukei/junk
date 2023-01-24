#CPPFLAGS=-g -Wall
CPPFLAGS=-O3 -Wall

TARGET=1_6_4
ALL_TARGET=1_6_2 1_6_4

all: $(TARGET)

clean:
	rm -f $(ALL_TARGET)

rebuild: clean all

run: all
	./$(TARGET)

debug: all
	gdb ./$(TARGET)

1_6_2:

1_6_4:

