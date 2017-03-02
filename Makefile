# -*- mode: Makefile; -*-

CXXFLAGS = -g -O -std=c++1z -W -Wall
LDFLAGS =

DEPENDDIR = ./.deps
DEPENDFLAGS = -M

SRCS := t.cpp
OBJS := $(patsubst %.cpp,%.o,$(SRCS))

TARGETS = $(patsubst %.o,%,$(OBJS))

all: $(TARGETS)

DEPS = $(patsubst %.o,$(DEPENDDIR)/%.d,$(OBJS))
-include $(DEPS)

$(DEPENDDIR)/%.d: %.c $(DEPENDDIR)
	$(CXX) $(CXXFLAGS) $(DEPENDFLAGS) $< >$@

$(DEPENDDIR):
	@[ ! -d $(DEPENDDIR) ] && mkdir -p $(DEPENDDIR)

%: %.cpp

%: %.o
	$(CXX) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -rf $(OBJS) $(TARGETS) $(DEPENDDIR)

