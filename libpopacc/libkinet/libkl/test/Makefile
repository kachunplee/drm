.SUFFIXES: .o .cpp

.cpp.o:
	$(CPP) $(CFLAGS) -c $<

.c.o:
	$(CC) $(CFLAGS) -c $<

#
RELEASE?=0

#
#
CFLAGS =
LFLAGS =
LIBS =

SRC_DIR=../../src
INET_DIR=..

#
#
#
CC=gcc
CPP=g++
CFLAGS +=-I.  -I.. -I../include
LFLAGS = -g -L.. -lkl

#
#
#
.if $(RELEASE) == "0"
CFLAGS += -g -O -Wall -DDEBUG $(OPTION)
.else
CFLAGS += -O3 -Wall $(OPTION)
.endif

make: progs

#
#
#
PROGS += newssubj

SRCS += newssubj.cpp 

NEWSSUBJ_OBJS= \
	newssubj.o

OBJS += $(NEWSSUBJ_OBJS)

newssubj: $(NEWSSUBJ_OBJS)
	$(CPP) -o $(.TARGET) $(.ALLSRC) $(LFLAGS) $(LIBS)

#
#
#

progs: $(PROGS)

clean:
	rm -f $(OBJS) $(PROGS)

depend:
	mkdep $(CFLAGS) $(SRCS)

# DO NOT DELETE THIS LINE -- make depend depends on it.
