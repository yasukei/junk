TESTS = money_test

CXXFLAGS+= -std=c++11
CPPFLAGS+= -I$(GTEST_DIR)/include

all : $(TESTS)

test : all
	./$(TESTS)

$(TESTS) : money_test.cpp money.cpp money.hpp gtest_main.a

clean :
	rm -f $(TESTS) gtest.a gtest_main.a *.o

#sample1.o : $(USER_DIR)/sample1.cc $(USER_DIR)/sample1.h $(GTEST_HEADERS)
#	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(USER_DIR)/sample1.cc
#
#sample1_unittest.o : $(USER_DIR)/sample1_unittest.cc \
#                     $(USER_DIR)/sample1.h $(GTEST_HEADERS)
#	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(USER_DIR)/sample1_unittest.cc
#
#sample1_unittest : sample1.o sample1_unittest.o gtest_main.a
#	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -lpthread $^ -o $@

include gtest.mk
