# -*- mode: Makefile; -*-

OSNAME = $(uname)

BENCHMARK_CPPFLAGS = -I/usr/local/include
BENCHMARK_LDFLAGS  = -L/usr/local/lib

BENCHMARK_LIBS = -lbenchmark

CXXFLAGS = -g -O -std=c++1z -W -Wall

ifeq ($(OSNAME),Linux)
  CXXFLAGS += -pthread
endif

CPPFLAGS = $(BENCHMARK_CPPFLAGS)
LDFLAGS  = $(BENCHMARK_LDFLAGS)

ifeq ($(OSNAME),Linux)
  LDFLAGS += -pthread
endif

LIBS = $(BENCHMARK_LIBS)

DEPENDDIR = ./.deps
DEPENDFLAGS = -M

SRCS := nfa.cpp dfa.cpp dot_graph.cpp
OBJS := $(patsubst %.cpp,%.o,$(SRCS))

TARGETS = t

all: $(TARGETS)

DEPS = $(patsubst %.o,$(DEPENDDIR)/%.d,$(OBJS))
-include $(DEPS)

$(DEPENDDIR)/%.d: %.cpp $(DEPENDDIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(DEPENDFLAGS) $< >$@

$(DEPENDDIR):
	@[ ! -d $(DEPENDDIR) ] && mkdir -p $(DEPENDDIR)

%: %.cpp

t: t.o $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -rf *.o $(OBJS) $(TARGETS) $(DEPENDDIR)

