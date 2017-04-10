# -*- mode: makefile -*-

EXTRA_DIST = 

include $(top_srcdir)/Makefile.common

bin_PROGRAMS = reta serial count-strings

reta_SOURCES = reta.cpp
reta_LDADD = $(top_srcdir)/src/libreta.la $(LIBS)

serial_SOURCES = serial.cpp
serial_LDADD = $(top_srcdir)/src/libreta.la $(LIBS)

count_strings_SOURCES = count-strings.cpp
count_strings_LDADD = $(top_srcdir)/src/libreta.la $(LIBS)
