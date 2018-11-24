SUBDIRS = .

EXTRA_DIST = run-test.sh

if WITH_CUTTER
noinst_LTLIBRARIES = test-argparser.la
endif

AM_CXXFLAGS = -std=c++17 -g3 -Wall $(CUTTER_CFLAGS) -I ../include

test_argparser_la_SOURCES = test-argparser.cc
AM_LDFLAGS = -module -rpath $(libdir) -avoid-version -no-undefined
LIBS = $(CUTTER_LIBS) $(CPPCUTTER_LIBS)

echo-cutter:
	@echo $(CUTTER)
