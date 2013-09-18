TARGET=\
	   junk_void\
	   junk_onStack\
	   junk_onData\
	   junk_onBss\


all: $(TARGET)
	sh list.sh

clean:
	rm -rf $(TARGET)

%: %.c
	$(CC) -O0 -o $@ $^

