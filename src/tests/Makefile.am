check_PROGRAMS = test_lessc

test_lessc_SOURCES = $(lessc_SOURCES) CssTokenizer_test.cpp	\
	CssParser_test.cpp LessParser_test.cpp

test_lessc_CXXFLAGS = -I$(GTEST_DIR)/include
test_lessc_LDADD = -lgtest 

TESTS = test_lessc
