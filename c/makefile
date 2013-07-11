TARGET=signal

all: $(TARGET)

clean:
	rm -rf $(TARGET)

%.o: %c
	(CC) -o $@ -c $<

%: %.o
	(CC) -o $@ $^

