.SUFFIXES: .o .cpp

.cpp.o:
	$(PCC) $(CFLAGS) -c $<

.c.o:
	$(CC) $(CFLAGS) -c $<

#
CC=gcc
PCC=g++

#
DEBUG?=1
#DEBUG?=0

OPTION=-I. -I../include

.if $(DEBUG) == "1"
CFLAGS=-g -Wall -DDEBUG $(OPTION)
LDFLAGS=-g
.else
CFLAGS=-O $(OPTION)
.endif

target: libzmime.a

SRCS = \
  zmime.cpp zm_msg.cpp \
  uudecode.cpp \
  ct.c header.c message.c mimemisc.c util.c

OBJS= \
  zmime.o zm_msg.o \
  uudecode.o \
  ct.o header.o message.o mimemisc.o util.o

libzmime.a: \
  $(OBJS)
	ar -r $(.TARGET) $(.ALLSRC)

depend:
	mkdep $(CFLAGS) $(SRCS)

clean:
	rm -f libzmime.a $(OBJS)

# DO NOT DELETE THIS LINE -- make depend depends on it.
