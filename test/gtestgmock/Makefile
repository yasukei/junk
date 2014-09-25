TEST_TARGET = ProductCodeTest

SRC_OBJS = \
	ProductCode.o\
	$(FRC)

TEST_OBJS = \
	ProductCode_test.o\
	$(FRC)

INCLUDE_DIRS = \
	.

CPPFLAGS += -I$(GTEST_DIR)/include -I$(GMOCK_DIR)/include -I$(INCLUDE_DIRS)
CXXFLAGS += -g -Wall -Wextra

all : $(TEST_TARGET)

clean :
	rm -f gmock.a gmock_main.a *.o
	rm -rf $(TEST_TARGET) $(SRC_OBJS) $(TEST_OBJS)

test : $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET) : gmock_main.a $(TEST_OBJS) $(SRC_OBJS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -lpthread -o $@

%.o : %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

#%.o : %.c
#	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

include gtestgmock.mk

