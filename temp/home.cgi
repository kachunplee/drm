#! /usr/bin/perl

use CGI qw(:standard);
use strict;

use drmdef;

my $BORDER = $DEBUG ? 1 : 0;

$| = 1;
print header;

my $CTRLHEIGHT = 80;
my $FOLDERWIDTH = 150;

my $LCTRLHEIGHT = 80;
my $LISTHEIGHT = '30%';

print <<EOM;
<frameset rows="$CTRLHEIGHT,*" framespacing=0 frameborder=$BORDER border=1>
	<frame name=DRMControl scrolling=no NORESIZE src=/mail/frmctrl>
	<frameset cols="$FOLDERWIDTH,*" framespacing=0 frameborder=$BORDER border=1>
		<frame name=DRMFolder src=/mail/frmfolder>
		<frame name=DRMList scrolling=no src=/mail/frmlist>
		<frameset rows="$LCTRLHEIGHT,$LISTHEIGHT,*" framespacing=0 frameborder=$BORDER border=1>
			<frame name=DRMLstControl scrolling=no NORESIZE src=/mail/frmlstctrl>
			<frame name=DRMLstDisplay src=/mail/in>
			<frame name=DRMLstPreview src=/mail/frmlstpreview>
		</frameset>
	</frameset>
</frameset>
EOM
