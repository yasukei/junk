TEST_TARGET = googleTestExampleC

TEST_DIR = test
TEST_FILES = $(shell find $(TEST_DIR) -name '*.cpp')
TEST_OBJS = $(TEST_FILES:.cpp=.o)

MOCK_DIR = mock
MOCK_FILES = $(shell find $(MOCK_DIR) -name '*.cpp')
MOCK_OBJS = $(MOCK_FILES:.cpp=.o)

SRC_DIR = src
SRC_FILES = $(shell find $(SRC_DIR) -name '*.c')
SRC_OBJS = $(SRC_FILES:.c=.o)

LCOV_DIR = lcov
LCOV_INFO = lcov.info

INCLUDE_DIRS = \
	-I$(TEST_DIR) \
	-I$(MOCK_DIR) \
	-I$(SRC_DIR) \
	-I$(GTEST_DIR)/include \
	-I$(GMOCK_DIR)/include \
	$(FRC)

DEBUG_OPTION = -g
COVERAGE_OPTION = -fprofile-arcs -ftest-coverage
WARNING_OPTION = -Wall -Wextra
COMPILE_OPTION = -std=c++11 -pthread

all : $(TEST_TARGET)

test : $(TEST_TARGET)
	./$(TEST_TARGET)

lcov : test
	mkdir -p $(LCOV_DIR)
	lcov -c -d . -o $(LCOV_DIR)/$(LCOV_INFO)
	genhtml -o $(LCOV_DIR) $(LCOV_DIR)/$(LCOV_INFO)

clean :
	rm -f *.o *.a
	rm -f $(TEST_TARGET) $(TEST_OBJS) $(MOCK_OBJS) $(SRC_OBJS)
	rm -rf $(LCOV_DIR) $(shell find . -name '*.gcda' -o -name '*.gcno')

$(TEST_OBJS): CPPFLAGS += $(INCLUDE_DIRS)
$(TEST_OBJS): CXXFLAGS += $(DEBUG_OPTION) $(WARNING_OPTION) $(COMPILE_OPTION)

$(MOCK_OBJS): CPPFLAGS += $(INCLUDE_DIRS)
$(MOCK_OBJS): CXXFLAGS += $(DEBUG_OPTION) $(WARNING_OPTION) $(COMPILE_OPTION)

$(SRC_OBJS): CPPFLAGS +=
$(SRC_OBJS): CFLAGS += $(DEBUG_OPTION) $(COVERAGE_OPTION) $(WARNING_OPTION)

$(TEST_TARGET): CPPFLAGS +=
$(TEST_TARGET): CXXFLAGS += $(DEBUG_OPTION) $(COVERAGE_OPTION) $(WARNING_OPTION) $(COMPILE_OPTION)
$(TEST_TARGET) : $(TEST_OBJS) $(MOCK_OBJS) $(SRC_OBJS) gmock_main.a
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@

include gtestgmock.mk

