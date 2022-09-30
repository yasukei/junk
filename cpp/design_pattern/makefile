
OBJS = \
	   $(addsuffix .o, $(basename $(wildcard *.cpp))) \
	   $(FRC)

all: $(OBJS)

clean:
	rm -f *.o

%.o: %.cpp
	$(CXX) -c $^

