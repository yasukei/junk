TARGET=statemachine
CXXFLAGS= -std=c++11

all: $(TARGET)

run: test

test: all
	./$(TARGET)

clean:
	rm -f $(TARGET)

$(TARGET): *.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@

