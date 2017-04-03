# -*- mode: makefile -*-

EXTRA_DIST = 

include $(top_srcdir)/Makefile.common

LIBS += $(top_srcdir)/src/libreta.la

if DARWIN
  # Because stupid
  LIBS += -lc++abi
endif

BENCHMARK_CPPFLAGS = -I/usr/local/include
BENCHMARK_LDFLAGS = -L/usr/local/lib

BENCHMARK_LIBS = -lbenchmark

bin_PROGRAMS = perf
perf_SOURCES = perf.cpp

perf_CPPFLAGS = $(AM_CPPFLAGS) $(BENCHMARK_CPPFLAGS)
perf_LDFLAGS = $(AM_LDFLAGS)  $(BENCHMARK_LDFLAGS)
perf_LDADD = $(BENCHMARK_LIBS) $(LIBS)

TESTS = construction
check_PROGRAMS = construction

construction_SOURCES = construction.cpp
construction_LDADD = $(BOOST_UNIT_TEST_FRAMEWORK_LIBS) $(LIBS) 
