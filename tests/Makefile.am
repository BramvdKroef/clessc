check_PROGRAMS = test_lessc

test_lessc_SOURCES = CssTokenizer_test.cpp CssParser_test.cpp	\
	LessParser_test.cpp $(top_builddir)/src/CssTokenizer.h	\
	$(top_builddir)/src/CssParser.h				\
	$(top_builddir)/src/LessParser.h

test_lessc_CXXFLAGS = -I$(GTEST_DIR)/include -I$(top_builddir)/src
test_lessc_LDADD = -lgtest $(top_builddir)/src/liblessc.a	\
	$(LIBPNG_LIBS) $(LIBJPEG_LIBS) -lgtest_main

TESTS = test_lessc
