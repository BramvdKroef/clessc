CXX = g++
CXXFLAGS = -Wall -g -Wextra
SRC = src
BIN = build
OBJS = $(BIN)/Token.o $(BIN)/TokenList.o $(BIN)/CssTokenizer.o \
	$(BIN)/CssParser.o $(BIN)/Stylesheet.o  $(BIN)/CssWriter.o \
	$(BIN)/LessTokenizer.o $(BIN)/LessParser.o $(BIN)/Value.o \
	$(BIN)/Color.o $(BIN)/ValueProcessor.o $(BIN)/Selector.o \
	$(BIN)/ParameterRuleset.o $(BIN)/CssPrettyWriter.o
EXEC = lessc
PREFIX = /usr/local

GTEST_DIR = gtest
CPPFLAGS += -I$(GTEST_DIR)/include
TEST_EXEC = unittest
TESTS = $(BIN)/CssTokenizer_test.o $(BIN)/CssParser_test.o
GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h

all : $(EXEC)

$(BIN) :
	mkdir $(BIN)

$(BIN)/%.o : $(SRC)/%.cpp $(SRC)/%.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(EXEC) : $(BIN) $(OBJS) $(BIN)/main.o
	$(CXX) $(CXXFLAGS) $(OBJS) $(BIN)/main.o -o $@

$(BIN)/main.o : $(SRC)/main.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

install : $(EXEC)
	install -m 0755 $(EXEC) $(PREFIX)/bin

clean:
	rm -f $(BIN)/*.o $(BIN)/*.a
	rmdir $(BIN)
	rm -f $(EXEC) $(TEST_EXEC)

# Google test rules
GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

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

# Compile test cases
$(BIN)/%_test.o : $(SRC)/tests/%_test.cpp $(SRC)/%.h $(GTEST_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

# Create unit test executable
$(TEST_EXEC) : $(OBJS) $(TESTS) $(BIN)/gtest_main.a 
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -lpthread $^ -o $@
