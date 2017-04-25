TEST_TARGET = googleTestExampleC

SRC_DIR = src
SRC_FILES = $(shell find $(SRC_DIR) -name *.c)
SRC_OBJS = $(SRC_FILES:.c=.o)

TEST_DIR = test
TEST_FILES = $(shell find $(TEST_DIR) -name *.cpp)
TEST_OBJS = $(TEST_FILES:.cpp=.o)

INCLUDE_DIRS = \
	-I$(GTEST_DIR)/include \
	-I$(GMOCK_DIR)/include \
	-Isrc/ \
	$(FRC)

CPPFLAGS += $(INCLUDE_DIRS)
CXXFLAGS += -g -Wall -Wextra -pthread

all : $(TEST_TARGET)

test : $(TEST_TARGET)
	./$(TEST_TARGET)

clean :
	rm *.o *.a
	rm $(TEST_TARGET) $(SRC_OBJS) $(TEST_OBJS)

$(TEST_TARGET) : gmock_main.a $(SRC_OBJS) $(TEST_OBJS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@

include gtestgmock.mk

