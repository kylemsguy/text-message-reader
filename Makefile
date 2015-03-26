CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS=-g -Wall
SRCS=readtxt.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

all: readtxt

readtxt: $(OBJS)
	$(CXX) $(LDFLAGS) -o tool $(OBJS) $(LDLIBS) 

clean:
	$(RM) $(OBJS)

dist-clean: clean
	$(RM) readtxt