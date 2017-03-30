# -*- mode: Makefile; -*-

BENCHMARK_CPPFLAGS = -I/usr/local/include
BENCHMARK_LDFLAGS  = -L/usr/local/lib

BENCHMARK_LIBS = -lbenchmark

CXXFLAGS = -g -O -std=c++1z -W -Wall -pthread

CPPFLAGS = $(BENCHMARK_CPPFLAGS)
LDFLAGS = -pthread $(BENCHMARK_LDFLAGS)

LIBS = $(BENCHMARK_LIBS)

DEPENDDIR = ./.deps
DEPENDFLAGS = -M

SRCS := t.cpp
OBJS := $(patsubst %.cpp,%.o,$(SRCS))

TARGETS = $(patsubst %.o,%,$(OBJS))

all: $(TARGETS)

DEPS = $(patsubst %.o,$(DEPENDDIR)/%.d,$(OBJS))
-include $(DEPS)

$(DEPENDDIR)/%.d: %.cpp $(DEPENDDIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(DEPENDFLAGS) $< >$@

$(DEPENDDIR):
	@[ ! -d $(DEPENDDIR) ] && mkdir -p $(DEPENDDIR)

%: %.cpp

%: %.o
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -rf $(OBJS) $(TARGETS) $(DEPENDDIR)

