CXX = g++
CXXFLAGS = -Wall -g -Wextra
SRC = src
BIN = build
OBJS = $(BIN)/Token.o $(BIN)/TokenList.o $(BIN)/CssTokenizer.o \
	$(BIN)/CssParser.o $(BIN)/Stylesheet.o  $(BIN)/CssWriter.o \
	$(BIN)/LessTokenizer.o $(BIN)/LessParser.o $(BIN)/Value.o \
	$(BIN)/Color.o $(BIN)/ValueProcessor.o $(BIN)/Selector.o \
	$(BIN)/ParameterRuleset.o $(BIN)/main.o
EXEC = lessc
PREFIX = /usr/local

GTEST_DIR = gtest
CPPFLAGS += -I$(GTEST_DIR)/include
TESTS = $(BIN)/CssTokenizer_test
GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h

all : $(EXEC)

$(BIN) :
	mkdir $(BIN)

$(BIN)/%.o : $(SRC)/%.cpp $(SRC)/%.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(EXEC) : $(BIN) $(OBJS) 
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@

$(BIN)/main.o : $(SRC)/main.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

install : $(EXEC)
	install -m 0755 $(EXEC) $(PREFIX)/bin

clean:
	rm -f $(BIN)/*.o 
	rmdir $(BIN)
	rm -f $(EXEC)


unittest : $(TESTS)
	$(TESTS)

GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

# For simplicity and to avoid depending on Google Test's
# implementation details, the dependencies specified below are
# conservative and not optimized.  This is fine as Google Test
# compiles fast and for ordinary users its source rarely changes.
$(BIN)/gtest-all.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest-all.cc -o $@

$(BIN)/gtest_main.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest_main.cc -o $@

$(BIN)/gtest.a : $(BIN)/gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

$(BIN)/gtest_main.a : $(BIN)/gtest-all.o $(BIN)/gtest_main.o
	$(AR) $(ARFLAGS) $@ $^

$(BIN)/CssTokenizer_test.o : $(SRC)/tests/CssTokenizer_test.cpp \
	$(SRC)/CssTokenizer.h $(GTEST_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c \
	$(SRC)/tests/CssTokenizer_test.cpp -o $@

$(BIN)/CssTokenizer_test : $(BIN)/CssTokenizer.o $(BIN)/Token.o \
	$(BIN)/CssTokenizer_test.o $(BIN)/gtest_main.a
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -lpthread $^ -o $@

