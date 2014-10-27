AUTOMAKE_OPTIONS = subdir-objects
AM_CPPFLAGS = -Wall $(LIBPNG_CFLAGS)

noinst_LIBRARIES = liblessc.a

liblessc_a_SOURCES = Token.cpp TokenList.cpp CssTokenizer.cpp		\
CssParser.cpp Stylesheet.cpp CssWriter.cpp LessTokenizer.cpp		\
LessParser.cpp Selector.cpp CssPrettyWriter.cpp				\
CssParser.h CssPrettyWriter.h CssTokenizer.h CssWriter.h		\
IOException.h LessParser.h LessTokenizer.h 				\
ParseException.cpp ParseException.h Selector.h Stylesheet.h Token.h	\
TokenList.h Mixin.cpp Mixin.h						\
UnprocessedStatement.cpp UnprocessedStatement.h LessStylesheet.cpp	\
LessStylesheet.h LessRuleset.cpp LessRuleset.h LessMediaQuery.h		\
LessMediaQuery.cpp MediaQueryRuleset.h MediaQueryRuleset.cpp		\
LessSelector.h LessSelector.cpp ExtendCssWriter.h ExtendCssWriter.cpp	\
									\
value/Value.cpp value/Color.cpp value/StringValue.cpp			\
value/NumberValue.cpp value/UnitValue.cpp value/BooleanValue.cpp	\
value/UrlValue.cpp value/FunctionLibrary.cpp value/ValueProcessor.cpp	\
value/BooleanValue.h value/Color.h value/FunctionLibrary.h		\
value/NumberValue.h value/StringValue.h value/UnitValue.h		\
value/UrlValue.h value/Value.h value/ValueException.h			\
value/ValueProcessor.h

bin_PROGRAMS = lessc
lessc_SOURCES = lessc.cpp 
lessc_LDADD = liblessc.a $(LIBPNG_LIBS) $(LIBJPEG_LIBS) $(LIBGLOG_LIBS)
