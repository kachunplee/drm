.SUFFIXES: .o .cpp

.cpp.o:
	$(CC) $(CFLAGS) -c $<

.c.o:
	gcc $(CFLAGS) -c $<

#
CC=g++

DEBUG?=1
#DEBUG?=0

SERVER=mailserv.newsadm.com

WWWDIR=/var/local/www/spamhippo

MAILBIN=drm-bin

BIN_DIR=$(WWWDIR)/$(MAILBIN)/
TEMPLATE_DIR=$(WWWDIR)/$(MAILBIN)/
HTML_DIR=$(WWWDIR)/html/drm5/
IMG_DIR=$(WWWDIR)/html/drm/img/
JS_DIR=$(WWWDIR)/html/drm/js/
CSS_DIR=$(WWWDIR)/html/drm/css/

IMG_SITE=$(WWWDIR)/html/drm/img/
SHARE_BIN=/var/local/bin/

SRC_DIR=../src
OPTION=-DMAIL_BIN=\"/$(MAILBIN)\"
OPTION+= -DUSENET_APPROVE

.if $(DEBUG) == "1"
CFLAGS=-g -Wall -DDEBUG $(OPTION)
LDFLAGS=-g
.else
CFLAGS=-O3 -Wall $(OPTION)
.endif

make: all

.include "../mk/makefile.inc"
