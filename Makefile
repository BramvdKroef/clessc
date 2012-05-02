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


$(BIN) :
	mkdir $(BIN)

$(BIN)/%.o : $(SRC)/%.cpp $(SRC)/%.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(EXEC) : $(BIN) $(OBJS) 
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BIN)/main.o : $(SRC)/main.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

install : $(EXEC)
	install -m 0755 $(EXEC) $(PREFIX)/bin

clean:
	rm -f $(BIN)/*.o 
	rmdir $(BIN)
	rm -f $(EXEC)


