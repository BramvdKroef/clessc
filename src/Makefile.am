AUTOMAKE_OPTIONS = subdir-objects
AM_CPPFLAGS = $(LIBPNG_CFLAGS)

bin_PROGRAMS = lessc
lessc_SOURCES = main.cpp Token.cpp TokenList.cpp CssTokenizer.cpp	\
CssParser.cpp Stylesheet.cpp CssWriter.cpp LessTokenizer.cpp		\
LessParser.cpp Selector.cpp ParameterRuleset.cpp CssPrettyWriter.cpp	\
ParameterRulesetLibrary.cpp value/Value.cpp value/Color.cpp		\
value/StringValue.cpp value/NumberValue.cpp value/UnitValue.cpp		\
value/BooleanValue.cpp value/UrlValue.cpp value/FunctionLibrary.cpp	\
value/ValueProcessor.cpp
lessc_LDADD = $(LIBPNG_LIBS) $(LIBJPEG_LIBS)
