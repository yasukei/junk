TARGET=signal

LDFLAGS+=-lpthread

all: $(TARGET)

$(TARGET) : signal.c
	$(CC) signal.c -o $@ -lpthread

clean:
	rm -rf $(TARGET)

run: all
	./$(TARGET)
