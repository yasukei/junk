TARGET=\
	   junk\


all: $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(TARGET)

%: %.c
	$(CC) -O0 -o $@ $^

