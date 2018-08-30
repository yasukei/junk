TARGET=main

LIB_DIR=lib

DEBUG_OPTION=-g
CFLAGS:=$(DEBUG_OPTION)
CFLAGS+=-I$(LIB_DIR)
LDFLAGS=-L$(LIB_DIR) -llib

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

clean:
	rm -f $(TARGET)

gdb: all
	LD_LIBRARY_PATH=.:$(LIB_DIR) gdb $(TARGET)

gdbclient: all
	LD_LIBRARY_PATH=.:$(LIB_DIR) gdb $(TARGET) -ex 'target remote localhost:9999' -ex 'b main' -ex 'c'

gdbserver: all
	LD_LIBRARY_PATH=.:$(LIB_DIR) gdbserver localhost:9999 ./$(TARGET)

