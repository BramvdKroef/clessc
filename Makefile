CC = g++
CFLAGS = -Wall -g
SRCDIR = src
ODIR = build
OBJS = $(ODIR)/Token.o $(ODIR)/TokenList.o $(ODIR)/CssTokenizer.o \
	$(ODIR)/CssParser.o $(ODIR)/Stylesheet.o  $(ODIR)/CssWriter.o \
	$(ODIR)/LessTokenizer.o $(ODIR)/LessParser.o $(ODIR)/Value.o \
	$(ODIR)/Color.o $(ODIR)/ValueProcessor.o $(ODIR)/Selector.o \
	$(ODIR)/ParameterRuleset.o $(ODIR)/main.o
EXEC = lessc
PREFIX = /usr/local
RM = rm

$(ODIR)/%.o : $(SRCDIR)/%.cpp $(SRCDIR)/%.h
	$(CC) $(CFLAGS) -c -o $@ $<

$(EXEC) : $(OBJS) 
	$(CC) $(CFLAGS) $^ -o $@

$(ODIR)/main.o : $(SRCDIR)/main.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

install : $(EXEC)
	install -m 0755 $(EXEC) $(PREFIX)/bin

clean:
	$(RM) $(ODIR)/*.o 
