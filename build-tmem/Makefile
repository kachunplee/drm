.SUFFIXES: .o .cpp

.cpp.o:
	$(CC) $(CFLAGS) -c $<

.c.o:
	gcc $(CFLAGS) -c $<

#
CC=g++

DEBUG?=1
#DEBUG?=0

SERVER=perry.newsadm.com

WWWDIR=/var/local/www/tmem.newsadm.com
MAILBIN=drm-bin

BIN_DIR=$(WWWDIR)/$(MAILBIN)/
TEMPLATE_DIR=$(WWWDIR)/$(MAILBIN)/
HTML_DIR=$(WWWDIR)/htdocs/drm5/
IMG_DIR=$(WWWDIR)/htdocs/drm/img/
JS_DIR=$(WWWDIR)/htdocs/drm/js/
CSS_DIR=$(WWWDIR)/htdocs/drm/css/

IMG_SITE=$(WWWDIR)/htdocs/drm/img/

SRC_DIR=../src
OPTION=-DMAIL_BIN=\"/$(MAILBIN)\"
OPTION=-DCMDGETALIAS=\"/var/local/spmz/spamhippo/bin/getalias\"
OPTION+= -DUSENET_APPROVE

.if $(DEBUG) == "1"
CFLAGS=-g -Wall -DDEBUG $(OPTION)
LDFLAGS=-g
.else
CFLAGS=-O3 -Wall $(OPTION)
.endif


make: all

.include "../mk/makefile.inc"
