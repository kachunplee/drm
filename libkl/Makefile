.SUFFIXES: .o .cpp

.cpp.o:
	$(PCC) $(CFLAGS) -c $<

.c.o:
	$(CC) $(CFLAGS) -c $<

#
CC=gcc
PCC=g++

DEBUG?=0

OPTION=-I. -Iinclude

.if $(DEBUG) == "1"
CFLAGS=-g -Wall -DDEBUG $(OPTION)
LDFLAGS=-g
.else
CFLAGS=-g -O $(OPTION)
LDFLAGS=-g
.endif

target=libkl.a

SRCS= \
	filelock.cpp \
	split.cpp \
	gethash.cpp \
	proctitle.cpp \
	display.cpp html.cpp status.cpp zcgi.cpp \
	newslib.cpp inlib.cpp \
	zstring.cpp zstg.cpp zstglib.cpp \
	dmsg.cpp zio.cpp linein.cpp \
	zrtn.cpp mkdir.cpp \
	e32.cpp \
	parsedate.c

OBJS= \
	filelock.o \
	split.o \
	gethash.o \
	proctitle.o \
	display.o html.o status.o zcgi.o \
	newslib.o inlib.o \
	zstring.o zstg.o zstglib.o \
	dmsg.o zio.o linein.o \
	zrtn.o mkdir.o \
	e32.o \
	parsedate.o

$(target): $(OBJS)
	ar -r $(.TARGET) $(.ALLSRC)

depend:
	mkdep $(CFLAGS) $(SRCS)

clean:
	rm -f $(target) $(OBJS)
