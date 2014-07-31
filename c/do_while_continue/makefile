TARGET=do_while_continue

all: $(TARGET)

$(TARGET) : $(TARGET).c
	$(CC) $^ -o $@

clean:
	rm -rf $(TARGET)

run: all
	./$(TARGET)
