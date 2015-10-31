SILENT=@

CFLAGS+=-Wall -fPIC
LDFLAGS+=-shared

TARGET=main
TARGET_FILES=main.c

LIBRARY1=libLibrary1.so
LIBRARY1_FILES=libLibrary1.c

all: clean $(LIBRARY1) $(TARGET)

$(TARGET): main.c

$(LIBRARY1): library1.c

clean:
	rm -f $(TARGET)

%.so:
	$(CC) $(CLFAGS) $(LDFLAGS) $^ -o $@

%.d: %.c
        $(SHELL) -ec '$(CC) -M $(CPPFLAGS) $< \
                      | sed '\"s/\($*\)\.o[ :]*/\1.o $@ : /g'\" > $@; \
                      [ -s $@ ] || rm -f $@'

