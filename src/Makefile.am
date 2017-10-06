AUTOMAKE_OPTIONS = subdir-objects
AM_CPPFLAGS = -Wall $(LIBPNG_CFLAGS) -std=c++11

noinst_LIBRARIES = liblessc.a

liblessc_a_SOURCES = \
Token.cpp				\
Token.h					\
TokenList.cpp				\
TokenList.h				\
VariableMap.cpp				\
VariableMap.h				\
stylesheet/AtRule.cpp			\
stylesheet/AtRule.h			\
stylesheet/CssComment.cpp		\
stylesheet/CssComment.h			\
stylesheet/CssWritable.h		\
stylesheet/Declaration.cpp		\
stylesheet/Declaration.h		\
stylesheet/MediaQuery.cpp		\
stylesheet/MediaQuery.h			\
stylesheet/Ruleset.cpp			\
stylesheet/Ruleset.h			\
stylesheet/RulesetStatement.cpp		\
stylesheet/RulesetStatement.h		\
stylesheet/Selector.cpp			\
stylesheet/Selector.h			\
stylesheet/Stylesheet.cpp		\
stylesheet/Stylesheet.h			\
stylesheet/StylesheetStatement.cpp	\
stylesheet/StylesheetStatement.h	\
css/CssParser.cpp			\
css/CssParser.h				\
css/CssPrettyWriter.cpp			\
css/CssPrettyWriter.h			\
css/CssTokenizer.cpp			\
css/CssTokenizer.h			\
css/CssWriter.cpp			\
css/CssWriter.h				\
css/IOException.h			\
css/ParseException.cpp			\
css/ParseException.h			\
css/SourceMapWriter.cpp			\
css/SourceMapWriter.h			\
lessstylesheet/LessAtRule.cpp		\
lessstylesheet/LessAtRule.h		\
lessstylesheet/LessMediaQuery.cpp	\
lessstylesheet/LessMediaQuery.h		\
lessstylesheet/LessRuleset.cpp		\
lessstylesheet/LessRuleset.h		\
lessstylesheet/LessSelector.cpp		\
lessstylesheet/LessSelector.h		\
lessstylesheet/LessStylesheet.cpp	\
lessstylesheet/LessStylesheet.h		\
lessstylesheet/MediaQueryRuleset.cpp	\
lessstylesheet/MediaQueryRuleset.h	\
lessstylesheet/Mixin.cpp		\
lessstylesheet/Mixin.h			\
lessstylesheet/UnprocessedStatement.cpp	\
lessstylesheet/UnprocessedStatement.h	\
lessstylesheet/Extension.cpp		\
lessstylesheet/Extension.h		\
lessstylesheet/ProcessingContext.cpp	\
lessstylesheet/ProcessingContext.h	\
lessstylesheet/Closure.cpp		\
lessstylesheet/Closure.h		\
lessstylesheet/MixinCall.h		\
lessstylesheet/MixinCall.cpp		\
less/LessParser.cpp			\
less/LessParser.h			\
less/LessTokenizer.cpp			\
less/LessTokenizer.h			\
value/BooleanValue.cpp			\
value/BooleanValue.h			\
value/BooleanValue.o			\
value/Color.cpp				\
value/Color.h				\
value/FunctionLibrary.cpp		\
value/FunctionLibrary.h			\
value/NumberValue.cpp			\
value/NumberValue.h			\
value/StringValue.cpp			\
value/StringValue.h			\
value/UnitValue.cpp			\
value/UnitValue.h			\
value/UrlValue.cpp			\
value/UrlValue.h			\
value/Value.cpp				\
value/Value.h				\
value/ValueException.h			\
value/ValueProcessor.cpp		\
value/ValueProcessor.h			\
value/ValueScope.h


bin_PROGRAMS = lessc
lessc_SOURCES = lessc.cpp 
lessc_LDADD = liblessc.a $(LIBPNG_LIBS) $(LIBJPEG_LIBS) $(LIBGLOG_LIBS)
