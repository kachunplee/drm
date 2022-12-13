mkdir:
	@set -x; for d in $(HTML_DIR) $(BIN_DIR) \\
		$(IMG_DIR) $(JS_DIR) $(CSS_DIR) \\
	; do if [ ! -d $$d ] ; then mkdir $$d ; chown http:WWW $$d ; fi ; done

DELETED=\
	$(BIN_DIR)frmcompose \

SHARE_SCRIPTS=\
	$(SHARE_BIN)expandalias.pl \

DRM_DEPS=\
	$(BIN_DIR)conf.ph

DRM_CSS=\

SITE_CSS=\
	$(CSS_DIR)drm.css $(CSS_DIR)drmmailto.css \
	$(CSS_DIR)drmlayout.css $(CSS_DIR)drmmenu.css \
	$(CSS_DIR)rte.css $(CSS_DIR)ktedit.css \

DRM_BINS=\
	$(BIN_DIR)folder \
	$(BIN_DIR)mailpost $(BIN_DIR)mailxfer \
	$(BIN_DIR)message $(BIN_DIR)compose \
	$(BIN_DIR)mailmark \
	$(BIN_DIR)drmfolder \

DRM_SCRIPTS=\
	$(BIN_DIR)frmctrl \
	$(BIN_DIR)frmlstctrl $(BIN_DIR)frmlstpreview \
	$(BIN_DIR)frmmessage $(BIN_DIR)frmmsgheader \
	\
	$(BIN_DIR)spellchk $(BIN_DIR)spelldone $(BIN_DIR)correct \
	$(BIN_DIR)mailsend $(BIN_DIR)adddct \
	$(BIN_DIR)prefspell $(BIN_DIR)drmspell $(BIN_DIR)drmspell.ph \
	\
	$(BIN_DIR)frmprnmsg $(BIN_DIR)frmprnmsghdr \
	\
	$(BIN_DIR)drmpref.ph $(BIN_DIR)drmpref \
	$(BIN_DIR)frmprefhd \
	\
	$(BIN_DIR)frmprefmn \
	$(BIN_DIR)frmprefpop \
	\
	$(BIN_DIR)drmaddbk $(BIN_DIR)frmaddbkhd \
	$(BIN_DIR)frmaddbkmn $(BIN_DIR)frmaddbked \
	\
	$(BIN_DIR)addfolder $(BIN_DIR)addfolderdir $(BIN_DIR)folder.ph \
	$(BIN_DIR)delfolder $(BIN_DIR)renamefolder \
	\
	$(BIN_DIR)drmdef.pm $(BIN_DIR)drmsettings.pm \
	\
	$(BIN_DIR)emptytrash $(BIN_DIR)emptyinbox \
	\
	$(BIN_DIR)folderopen \
	\
	$(BIN_DIR)errmsg.ph

DRM_SUSCRIPTS=\
	$(BIN_DIR)wwwmail \


DRM_TEMPLATE=\
	$(TEMPLATE_DIR)delfolderconfirm.html \
	$(TEMPLATE_DIR)delfoldererror.html \
	$(TEMPLATE_DIR)renamefolder.html \
	$(TEMPLATE_DIR)renamefoldererror.html \
	$(TEMPLATE_DIR)emptytrashconfirm.html \
	$(TEMPLATE_DIR)emptytrasherror.html \
	$(TEMPLATE_DIR)emptyinboxconfirm.html \
	$(TEMPLATE_DIR)emptyinboxerror.html \
	$(TEMPLATE_DIR)addfolder.html \
	$(TEMPLATE_DIR)addfolderdir.html \


DRM_HTMLS=\
	$(HTML_DIR)frmfolder.html \
	$(HTML_DIR)drmhelp.asp $(HTML_DIR)prefhelp.html \
	$(HTML_DIR)addbkhelp.html $(HTML_DIR)helpmain.html \
	$(HTML_DIR)helpheader.asp $(HTML_DIR)helpfilter.html \
	$(HTML_DIR)drmfilterhelp.html $(HTML_DIR)helpfilterheader.html \
	$(HTML_DIR)blank.html $(HTML_DIR)palette.html \
	\
	$(HTML_DIR)index.asp \
	$(HTML_DIR)drmprevw.asp \
	$(HTML_DIR)drmmenu.inc $(HTML_DIR)drmfolder.inc \
	\
	$(HTML_DIR)getaddbk.asp $(HTML_DIR)gettz2.asp \

SITE_HTMLS=\
	$(HTML_DIR)drmdef.inc \


DRM_IMGS=\
	$(IMG_DIR)bluego.gif $(IMG_DIR)boxbar.gif $(IMG_DIR)checkbar.gif \
	$(IMG_DIR)drmlogo.gif $(IMG_DIR)drnbottom.gif \
	$(IMG_DIR)logo_logout.gif \
	$(IMG_DIR)drnline.gif $(IMG_DIR)edrnlogo.gif $(IMG_DIR)horzbar.gif \
	$(IMG_DIR)folderclosed.gif $(IMG_DIR)folderopen.gif \
	$(IMG_DIR)folderdiropen.gif $(IMG_DIR)addfolderdir.gif \
	$(IMG_DIR)mailattach.gif $(IMG_DIR)mailattachblank.gif \
	$(IMG_DIR)mailbar.gif $(IMG_DIR)mailclosed.gif \
	$(IMG_DIR)mailforward.gif $(IMG_DIR)mailopened.gif \
	$(IMG_DIR)mailreply.gif $(IMG_DIR)mailsend.gif \
	$(IMG_DIR)mailtool.gif $(IMG_DIR)newmail.wav \
	$(IMG_DIR)sendbar.gif $(IMG_DIR)sendbut.gif \
	$(IMG_DIR)spellcheck.gif $(IMG_DIR)toolbar.gif \
	$(IMG_DIR)drmselectall.gif $(IMG_DIR)drmresetall.gif \
	$(IMG_DIR)vertical_blue.gif $(IMG_DIR)blue_selector.gif \
	$(IMG_DIR)spacer_1pixel.gif $(IMG_DIR)spacer_2pixel.gif \
	$(IMG_DIR)spacer_transparent.gif $(IMG_DIR)spacerwhite.gif \
	$(IMG_DIR)openmsg.gif $(IMG_DIR)drmtool_spacer3.gif \
	$(IMG_DIR)drmtool_verticalendcap.gif \
	$(IMG_DIR)toolcheckmail.gif $(IMG_DIR)toolnewmessage.gif \
	$(IMG_DIR)toolpreferences.gif $(IMG_DIR)toolfilters.gif \
	$(IMG_DIR)tooladdbook.gif $(IMG_DIR)toolfilters.gif \
	$(IMG_DIR)toolhelp.gif $(IMG_DIR)toolreplymessage.gif \
	$(IMG_DIR)toolreplyall.gif \
	$(IMG_DIR)toolforwardmessage.gif $(IMG_DIR)toolviewmessage.gif \
	$(IMG_DIR)toolviewheader.gif $(IMG_DIR)toolviewsource.gif \
	$(IMG_DIR)toolapprovemessage.gif \
	$(IMG_DIR)toolsendmessage.gif $(IMG_DIR)toolspellcheck.gif \
	$(IMG_DIR)bluequestionmark.gif $(IMG_DIR)bluesave.gif \
	$(IMG_DIR)bluecancel.gif $(IMG_DIR)logo2_popup.gif \
	$(IMG_DIR)bluecreate.gif $(IMG_DIR)reddot.gif \
	$(IMG_DIR)spacer_yellow.gif $(IMG_DIR)spacer_darkyellow.gif \
	$(IMG_DIR)bluerename.gif $(IMG_DIR)bluedelete.gif \
	$(IMG_DIR)blueback.gif $(IMG_DIR)blueclose.gif \
	$(IMG_DIR)blueempty.gif $(IMG_DIR)blueoptionslog.gif \
	$(IMG_DIR)blueoptions.gif $(IMG_DIR)blueaddaddress.gif \
	$(IMG_DIR)toolverticalendcap.gif $(IMG_DIR)toolfiltershelp.gif \
	$(IMG_DIR)toolfiltersadd.gif $(IMG_DIR)toolfiltersviewlog.gif \
	$(IMG_DIR)toolfilterscurrent.gif $(IMG_DIR)blueoptionsfilter.gif \
	$(IMG_DIR)optionsnick.gif $(IMG_DIR)spacerwhite.gif \
	$(IMG_DIR)drmtool_pref_details.gif \
	$(IMG_DIR)drmtool_pref_addressbook.gif \
	$(IMG_DIR)drmtool_pref_help.gif \
	$(IMG_DIR)drmtool_pref_editaccounts.gif \
	$(IMG_DIR)drmtool_addbook_newcontact.gif \
	$(IMG_DIR)drmtool_addbook_help.gif \
	$(IMG_DIR)helpmain1.gif $(IMG_DIR)helpmain3.gif \
	$(IMG_DIR)helpmain4.gif $(IMG_DIR)helpmain5.gif \
	$(IMG_DIR)helpmain7.gif $(IMG_DIR)helpmain8.gif \
	$(IMG_DIR)helpmain9.gif $(IMG_DIR)helpmain10.gif \
	$(IMG_DIR)helpbulletattach.gif \
	$(IMG_DIR)helpbulletunread.gif \
	$(IMG_DIR)helpbulletopen.gif \
	$(IMG_DIR)helpbulletfwd.gif \
	$(IMG_DIR)helpbulletreply.gif  \
	$(IMG_DIR)helpbulletsent.gif \
	$(IMG_DIR)addfolder.gif $(IMG_DIR)emptytrash.gif \
	$(IMG_DIR)emptyinbox.gif \
	$(IMG_DIR)drm_icon_addcontact.gif \
	$(IMG_DIR)drm_spamhippo.gif \
	$(IMG_DIR)icon_addnewfolder.gif $(IMG_DIR)icon_emptytrash.gif \
	$(IMG_DIR)icon_messagestatus.gif \
	$(IMG_DIR)prnver.gif $(IMG_DIR)prnnow.gif \
	$(IMG_DIR)membership_drive3.gif \
	$(IMG_DIR)blanklogo.gif $(IMG_DIR)copyright.gif \
	$(IMG_DIR)drmedit.gif $(IMG_DIR)drmlinkline.gif \
	$(IMG_DIR)drmsave.gif $(IMG_DIR)drmsavesettings.gif \
	$(IMG_DIR)drnlogo.gif $(IMG_DIR)drnspellcancel.gif \
	$(IMG_DIR)drnspelldone.gif $(IMG_DIR)filter1.gif \
	$(IMG_DIR)maildelete.gif $(IMG_DIR)mailedit.gif \
	$(IMG_DIR)mailmoveup.gif $(IMG_DIR)navigate.gif \
	$(IMG_DIR)reddot.gif \
	$(IMG_DIR)spacer.gif $(IMG_DIR)spamming.gif \
	$(IMG_DIR)spellques.gif $(IMG_DIR)titlemailsettings.gif \
	$(IMG_DIR)transfericon.gif \
	$(IMG_DIR)blue_arrowdown.gif $(IMG_DIR)blue_arrowup.gif \
	$(IMG_DIR)dirclose.gif $(IMG_DIR)diropen.gif \
	\
	$(IMG_DIR)drmhelp_graphic1.gif $(IMG_DIR)drmhelp_graphic2.gif \
	$(IMG_DIR)drmhelp_graphic3.gif $(IMG_DIR)drmhelp_folderclosed.gif \
	$(IMG_DIR)drmhelp_foldertrans.gif $(IMG_DIR)drmhelp_graphic4.gif \
	$(IMG_DIR)drmhelp_graphic5.gif $(IMG_DIR)drmhelp_graphic6.gif \
	$(IMG_DIR)drmhelp_graphic7.gif $(IMG_DIR)drmhelp_graphic8.gif \
	$(IMG_DIR)drmhelp_folderopen.gif \
	\
	$(IMG_DIR)changeto_enter.gif $(IMG_DIR)changeto_cancel.gif \

SITE_IMGS=\
	$(IMG_DIR)toolbartop.gif $(IMG_DIR)titlelogo.gif \
	$(IMG_DIR)stitlelogo.gif \
	$(IMG_DIR)menubg.gif \

DRM_JS=\
	$(JS_DIR)drmwin.js \
	$(JS_DIR)floatmenu.js $(JS_DIR)foldermenu.js \
	$(JS_DIR)addrbook.js $(JS_DIR)drmmailto.js $(JS_DIR)drmpref.js \
	\
	$(JS_DIR)Helpers.js $(JS_DIR)NumberHelper.js $(JS_DIR)StringHelper.js \
	\
	$(JS_DIR)Ajax.js $(JS_DIR)HTTP.js \
	\
	$(JS_DIR)drmlist.js $(JS_DIR)drmfun.js \
	$(JS_DIR)drmfloat.js $(JS_DIR)dropmenu.js \
	$(JS_DIR)drmfolder.js $(JS_DIR)drmdrag.js \
	$(JS_DIR)drmwinadj.js $(JS_DIR)drmresize.js \
	\
	$(JS_DIR)postwinadj.js \
	\
	$(JS_DIR)richtext.js $(JS_DIR)ajax.js \
	$(JS_DIR)ktedit.js $(JS_DIR)drmedit.js \

DRM_UTILS=\
	/var/local/sbin/updatedict

install: $(DRM_DEPS) $(DRM_CSS) $(SITE_CSS) $(DRM_BINS) \
	$(DRM_SCRIPTS) $(DRM_SUSCRIPTS) $(DRM_TEMPLATE) \
	$(DRM_HTMLS) $(SITE_HTMLS) $(DRM_IMGS) $(SITE_IMGS) \
	$(DRM_JS) $(SITE_JS) $(DRM_UTILS)\
	$(SHARE_SCRIPTS) \
	$(BIN_DIR)folderclose \

$(DRM_BINS): ./${*F}
	install -c -o root -g WWW -m 04555 -s ${*F} $(*D)/

$(DRM_DEPS): $(*F)
	install -c -o http -g WWW $(*F) $(*D)/

$(DRM_SCRIPTS): ../mail-bin/$(*F)
	install -c -o http -g WWW ../mail-bin/$(*F) $(*D)/

$(DRM_SUSCRIPTS): ../mail-bin/$(*F)
	install -c -o http -g mail -m 02555 ../mail-bin/$(*F) $(*D)/

$(DRM_TEMPLATE): ../template/$(*F)
	install -c -o http -g WWW ../template/$(*F) $(*D)/

$(DRM_HTMLS): ../html/$(*F)
	install -c -o http -g WWW ../html/$(*F) $(*D)/

$(SITE_HTMLS): ./$(*F)
	install -c -o http -g WWW ./$(*F) $(*D)/

$(DRM_IMGS): ../images/$(*F)
	install -c -o http -g WWW ../images/$(*F) $(*D)/

$(SITE_IMGS): site/images/$(*F)
	install -c -o http -g WWW site/images/$(*F) $(*D)/

$(DRM_JS): ../js/$(*F)
	install -c -o http -g WWW ../js/$(*F) $(*D)/

$(DRM_CSS): ../css/$(*F)
	install -c -o http -g WWW ../css/$(*F) $(*D)/

$(SITE_CSS): site/css/$(*F)
	install -c -o http -g WWW site/css/$(*F) $(*D)/

/var/local/sbin/updatedict: ../ktspell/util/updatedict
	install -o bin -g bin ${.ALLSRC} $(*D)/

$(SHARE_SCRIPTS): ../bin/${*F}
	install -c -o bin -g bin -m 0555 ../bin/${*F} $(*D)/

$(BIN_DIR)folderclose: $(BIN_DIR)folderopen
	ln -f $(BIN_DIR)folderopen $(BIN_DIR)folderclose

#
CFLAGS += -I. -I../include -I../zmime -I../libpopacc/include -I../libkinet/include -I../libkl/include
LIBS=-L../zmime -lzmime -L../libpopacc -lpopacc -L../libkinet -lkinet -L../libkl -lkl

COMMON_SRC= \
	$(SRC_DIR)/mailopt.cpp $(SRC_DIR)/mailbox.cpp $(SRC_DIR)/mboxlock.cpp \
	$(SRC_DIR)/inbox.cpp \
	$(SRC_DIR)/htmllib.cpp $(SRC_DIR)/urllib.cpp \
	$(SRC_DIR)/mimetext.cpp \
	$(SRC_DIR)/wildmat.c $(SRC_DIR)/version.cpp

SRCS+=$(COMMON_SRC)

COMMON_OBJ= \
	mailopt.o mailbox.o mboxlock.o inbox.o \
	htmllib.o urllib.o \
	mimetext.o \
	wildmat.o version.o

OBJS+=$(COMMON_OBJ)

#
#
#
FOLDER_SRC= \
	$(SRC_DIR)/maillist.cpp $(SRC_DIR)/maillhtml.cpp \
	$(SRC_DIR)/maillxml.cpp $(SRC_DIR)/mailldat.cpp \
	$(SRC_DIR)/mailmsg.cpp \
	$(SRC_DIR)/mailto.cpp $(SRC_DIR)/mailsettings.cpp \
	$(SRC_DIR)/folder.cpp \

FOLDER_OBJ= \
	maillist.o maillhtml.o maillxml.o mailldat.o \
	mailmsg.o mailto.o mailsettings.o \
	folder.o \

PROGS+=folder
SRCS+=$(FOLDER_SRC)
OBJS+=$(FOLDER_OBJ)

folder: $(FOLDER_OBJ) $(COMMON_OBJ)
	$(CC) $(CFLAGS) -o $@ $(.ALLSRC) $(LIBS)

#
FCNT_SRC= $(SRC_DIR)/drmfolder.cpp
FCNT_OBJ= drmfolder.o

PROGS+=drmfolder
SRCS+=$(FCNT_SRC)
OBJS+=$(FCNT_OBJ)

drmfolder: $(FCNT_OBJ)
	$(CC) $(CFLAGS) -o $@ $(.ALLSRC) $(LIBS)

#
#
#
MESSAGE_SRC= \
	$(SRC_DIR)/mailmsg.cpp \
	$(SRC_DIR)/mailto.cpp $(SRC_DIR)/mailsettings.cpp \
	$(SRC_DIR)/message.cpp

MESSAGE_OBJ= \
	mailmsg.o \
	mailto.o mailsettings.o message.o

PROGS+=message
SRCS+=$(MESSAGE_SRC)
OBJS+=$(MESSAGE_OBJ)

message: $(MESSAGE_OBJ) $(COMMON_OBJ)
	$(CC) $(CFLAGS) -o $@ $(.ALLSRC) $(LIBS)

#
#
COMPOSE_SRC= \
	$(SRC_DIR)/mailmsg.cpp \
	$(SRC_DIR)/mailto.cpp $(SRC_DIR)/mailsettings.cpp \
	$(SRC_DIR)/compose.cpp \

COMPOSE_OBJ= \
	mailmsg.o \
	mailto.o mailsettings.o \
	compose.o

PROGS+=compose
SRCS+=$(COMPOSE_SRC)
OBJS+=$(COMPOSE_OBJ)

compose: $(COMPOSE_OBJ) $(COMMON_OBJ)
	$(CC) $(CFLAGS) -o $@ $(.ALLSRC) $(LIBS)

#
#
#
MAILPOST_SRC= \
	$(SRC_DIR)/cgi.cpp $(SRC_DIR)/httpbox.cpp \
	$(SRC_DIR)/mailmsg.cpp \
	$(SRC_DIR)/mailpost.cpp

MAILPOST_OBJ= \
	cgi.o httpbox.o \
	mailmsg.o mailsettings.o mailpost.o

PROGS+=mailpost
SRCS+=$(MAILPOST_SRC)
OBJS+=$(MAILPOST_OBJ)

mailpost: $(MAILPOST_OBJ) $(COMMON_OBJ)
	$(CC) $(CFLAGS) -o $@ $(.ALLSRC) $(LIBS)

#
#
#
MAILXFER_SRC= \
	$(SRC_DIR)/cgi.cpp $(SRC_DIR)/httpbox.cpp \
	$(SRC_DIR)/mailxfer.cpp \

MAILXFER_OBJ= \
	cgi.o httpbox.o \
	mailxfer.o

PROGS+=mailxfer
SRCS+=$(MAILXFER_SRC)
OBJS+=$(MAILXFER_OBJ)

mailxfer: $(MAILXFER_OBJ) $(COMMON_OBJ)
	$(CC) $(CFLAGS) -o $@ $(.ALLSRC) $(LIBS)

MAILMARK_SRC= \
	$(SRC_DIR)/cgi.cpp $(SRC_DIR)/httpbox.cpp \
	$(SRC_DIR)/mailmark.cpp \

MAILMARK_OBJ= \
	cgi.o httpbox.o \
	mailmark.o

PROGS+=mailmark
SRCS+=$(MAILMARK_SRC)
OBJS+=$(MAILMARK_OBJ)

mailmark: $(MAILMARK_OBJ) $(COMMON_OBJ)
	$(CC) $(CFLAGS) -o $@ $(.ALLSRC) $(LIBS)

#
all: $(PROGS)

tags:
	ctags ../src/*.cpp ../include/*.h ../zmime/*.cpp ../zmime/*.c ../zmime/*.h

update:
	rsh $(SERVER) "set -x; cd $(.CURDIR); make install"

depend:
	mkdep $(CFLAGS) $(SRCS)

clean:
	rm -f $(PROGS) $(OBJS)
