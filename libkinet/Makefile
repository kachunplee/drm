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

OPTION=-I. -Iinclude -Ilibkl/include

.if $(DEBUG) == "1"
CFLAGS=-g -O -Wall -DDEBUG $(OPTION)
LDFLAGS=-g
.else
CFLAGS=-O2 $(OPTION)
.endif

target: libkinet.a

SRCS = \
	pop3act.cpp cpop3.cpp \
	clientnntp.cpp \
	nntpact.cpp nntpov.cpp nntpart.cpp cnntp.cpp \
	clienttcp.cpp inettcp.cpp \
	clientudp.cpp inetudp.cpp rtt.cpp \
	cinet.cpp

OBJS= \
	pop3act.o cpop3.o \
	clientnntp.o \
	nntpact.o nntpov.o nntpart.o cnntp.o \
	clienttcp.o inettcp.o \
	clientudp.o inetudp.o cinet.o rtt.o

libkinet.a: \
  $(OBJS)
	ar -r $(.TARGET) $(.ALLSRC)

depend:
	mkdep $(CFLAGS) $(SRCS)

clean:
	rm -f libkinet.a $(OBJS)

# DO NOT DELETE THIS LINE -- make depend depends on it.
