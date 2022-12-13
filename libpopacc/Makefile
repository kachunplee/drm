.SUFFIXES: .o .cpp

.cpp.o:
	$(PCC) $(CFLAGS) -c $<

.c.o:
	$(CC) $(CFLAGS) -c $<

#
CC=gcc
PCC=g++

DEBUG?=0

OPTION=-I. -Iinclude -Ilibkinet/include -Ilibkl/include

.if $(DEBUG) == "1"
CFLAGS=-g -Wall -DDEBUG $(OPTION)
LDFLAGS=-g
.else
CFLAGS=-g -O $(OPTION)
LDFLAGS=-g
.endif

target=libpopacc.a

SRCS= \
	poprelay.cpp \
	cpopacc.cpp \

OBJS= \
	poprelay.o \
	cpopacc.o \

$(target): $(OBJS)
	ar -r $(.TARGET) $(.ALLSRC)

depend:
	mkdep $(CFLAGS) $(SRCS)

clean:
	rm -f $(target) $(OBJS)
