$DEBUG = 1;

$SERVICENAME = 'Newsadm';
$HOMEPAGE = 'http://newsadm.com';
$DRMServer = 'tdrm.newsadm.com';

$SettingsDir = '/var/local/mail/';
$MAILDIR     = '/var/local/mail/';

$MEMDRIVEURL = 'https://acc.newsadm.com/a/memsubmit.asp';

$BINDIR = '/drm-bin/';
$IMGDIR = '/drm/img/';
$CSSDIR = '/drm/css/';
$JSDIR = '/drm/js/';
$HTDIR = '/drm5/';

$DICTSERVER = 'tdrn.newsadm.com';	# For global dictionary
$DRNSERVER = 'tdrn.newsadm.com';
$MASTERID = 'tnewsadm';

sub GotPasswdChg
{
	return 0;
}

1;
