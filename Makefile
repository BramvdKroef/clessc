CC = g++
CFLAGS = -Wall
SRCDIR = src
ODIR = build
OBJS = $(ODIR)/Token.o $(ODIR)/TokenList.o $(ODIR)/CssTokenizer.o \
	$(ODIR)/CssParser.o $(ODIR)/Stylesheet.o  $(ODIR)/CssWriter.o \
	$(ODIR)/LessTokenizer.o $(ODIR)/LessParser.o $(ODIR)/Value.o \
	$(ODIR)/ValueProcessor.o $(ODIR)/main.o
EXEC = Interpreter
RM = rm

$(ODIR)/%.o : $(SRCDIR)/%.cpp $(SRCDIR)/%.h
	$(CC) $(CFLAGS) -c -o $@ $<

$(EXEC) : $(OBJS) 
	$(CC) $(CFLAGS) $^ -o $@

$(ODIR)/main.o : $(SRCDIR)/main.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) $(ODIR)/*.o 
