TARGET=main
TARGET_LIB=libmemmgr.so

all: $(TARGET_LIB) $(TARGET)

$(TARGET): main.c
	$(CC) $^ -o $@

$(TARGET_LIB): memorymanager.c
	$(CC) $^ -shared -fPIC -ldl -o $@

clean:
	rm -f $(TARGET_LIB) $(TARGET)

run: all
	LD_PRELOAD=`pwd`/$(TARGET_LIB) ./$(TARGET)

