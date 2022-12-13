$DEBUG = 1;

$SERVICENAME = 'DRM';
$HOMEPAGE = 'http://doc.kll.com';
$DRMServer = 'doc.kll.com';

$SettingsDir = '/var/local/mail/';
$MAILDIR     = '/var/local/mail/';

$MEMDRIVEURL = '';

$BINDIR = '/drm-bin/';
$IMGDIR = '/drm/img/';
$CSSDIR = '/drm/css/';
$JSDIR = '/drm/js/';
$HTDIR = '/drm5/';

$DICTSERVER = '';	# For global dictionary
$DRNSERVER = '';
$MASTERID = '';

sub GotPasswdChg
{
	my ($user, $passwd) = @_;

	return 1 if $passwd eq '';

	return "" if system("set -x; /var/local/bin/chgwhostpass $user $passwd 1>&2") == 0;
	return "Cannot change password... please email support\@newsadm.com for more info";
}
1;
