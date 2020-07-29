TARGET=eintr

LDFLAGS+=-lpthread

all: $(TARGET)

$(TARGET) : eintr.c
	$(CC) eintr.c -o $@ -lpthread

clean:
	rm -rf $(TARGET)

run: all
	./$(TARGET)
