$DEBUG = 0;

$SERVICENAME = 'Newsadm';
$HOMEPAGE = 'http://newsadm.com';
$DRMServer = 'mailserv.newsadm.com';

$SettingsDir = '/var/local/mail/';
$MAILDIR     = '/var/local/mail/';

$MEMDRIVEURL = 'https://acc.newsadm.com/a/memsubmit.asp';

$BINDIR = '/drm-bin/';
$IMGDIR = '/drm/img/';
$CSSDIR = '/drm/css/';
$JSDIR = '/drm/js/';
$HTDIR = '/drm5/';

$DICTSERVER = 'drn.newsadm.com';	# For global dictionary
$DRNSERVER = 'drn.newsadm.com';
$MASTERID = 'newsadm';

sub GotPasswdChg
{
	my ($user, $passwd) = @_;

	return 1 if $passwd eq '';

	return "" if system("set -x; /var/local/bin/chgwhostpass $user $passwd 1>&2") == 0;
	return "Cannot change password... please email support\@newsadm.com for more info";
}

1;
