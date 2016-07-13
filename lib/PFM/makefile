# targets.

CXXFLAGS= -std=c++11
LDLIBS= -lstdc++

TARGET = PFManager

OBJS = \
	PFManager.o \
	PFMprofile.o \
	$(FRC)

all : $(TARGET)

rebuild : clean all

run : all
	./$(TARGET)

$(TARGET) : $(OBJS)

clean :
	rm -rf $(TARGET) $(OBJS)

