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
CFLAGS +=-I.  -I.. -I../include -I../libkl/include
LFLAGS = -g -L.. -lkinet -L../libkl -lkl

#
#
#
.if $(RELEASE) == "0"
CFLAGS += -g -O -Wall -DDEBUG $(OPTION)
.else
CFLAGS += -O3 -Wall $(OPTION)
.endif

INET_SRCS =\
	$(INET_DIR)/nntpact.cpp $(INET_DIR)/nntpov.cpp \
	$(INET_DIR)/nntpart.cpp $(INET_DIR)/cnntp.cpp

INET_OBJS =\
	nntpact.o nntpov.o nntpart.o cnntp.o \

SRCS += $(INET_SRCS)
OBJS += $(INET_OBJS)

make: nntptest acttest ovtest arttest posttest

#
#
#
PROGS += nntptest

SRCS += nntptest.cpp 

NNTPTEST_OBJS= \
	nntptest.o cnntp.o 

OBJS += $(NNTPTEST_OBJS)

nntptest: $(NNTPTEST_OBJS)
	$(CPP) -o $(.TARGET) $(.ALLSRC) $(LFLAGS) $(LIBS)

#
#
#
PROGS += ovtest

SRCS += ovtest.cpp

OVTEST_OBJS= \
	ovtest.o

OBJS += $(OVTEST_OBJS)

ovtest: $(OVTEST_OBJS) nntpov.o cnntp.o
	$(CPP) -o $(.TARGET) $(.ALLSRC) $(LFLAGS) $(LIBS)

#
#
#
PROGS += acttest

SRCS += acttest.cpp

ACTTEST_OBJS= \
	acttest.o

OBJS += $(ACTTEST_OBJS)

acttest: $(ACTTEST_OBJS) nntpact.o cnntp.o
	$(CPP) -o $(.TARGET) $(.ALLSRC) $(LFLAGS) $(LIBS)

#
#
#
PROGS += arttest

SRCS += arttest.cpp

ARTTEST_OBJS= \
	arttest.o

OBJS += $(ARTTEST_OBJS)

arttest: $(ARTTEST_OBJS) nntpart.o cnntp.o
	$(CPP) -o $(.TARGET) $(.ALLSRC) $(LFLAGS) $(LIBS)

#
#
#
PROGS += posttest

SRCS += posttest.cpp

POSTTEST_OBJS= \
	posttest.o

OBJS += $(POSTTEST_OBJS)

posttest: $(POSTTEST_OBJS) cnntp.o
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
