# -*- mode: makefile -*-

EXTRA_DIST = 

include $(top_srcdir)/Makefile.common

bin_PROGRAMS = reta serial

reta_SOURCES = reta.cpp
reta_LDADD = $(top_srcdir)/src/libreta.la $(LIBS)

serial_SOURCES = serial.cpp
serial_LDADD = $(top_srcdir)/src/libreta.la $(LIBS)
