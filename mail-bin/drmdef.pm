package drmdef;

use vars qw(@ISA @EXPORT);

use Exporter;
@ISA = qw(Exporter);

@EXPORT =
   qw(
	$VERSION
	$MAXFOLDER
	$MEMDRIVEURL

	$SERVICENAME
	$HOMEPAGE
	$DRMServer
	$SettingsDir
	$MAILDIR
	$HTDIR
	$BINDIR
	$IMGDIR
	$CSSDIR
	$JSDIR

	$DICTSERVER
	$DRNSERVER
	$MASTERID

	&GotPasswdChg
   );

$VERSION="5.11";
$MAXFOLDER = 500;

require "conf.ph";

1;
