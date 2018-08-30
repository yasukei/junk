TARGET_LIB=liblib.so

DEBUG_OPTION=-g
#DEBUG_OPTION=
CFLAGS:=$(DEBUG_OPTION)
CFLAGS+=-shared -fPIC

all: $(TARGET_LIB)

$(TARGET_LIB): lib.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $(TARGET_LIB)

clean:
	rm -f $(TARGET_LIB)

rebuild: clean all

