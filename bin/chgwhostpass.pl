#! /usr/bin/perl

use strict;

use lib qw(/var/local/libdata/perl5 /var/local/lib/perl5);

use LOCKF;
use mail_err;

use vars qw($AdminMailAddr);
$AdminMailAddr = "kachun";

my $sh = "/nonexistent";

my $mx_server = '';

my $dbmpasswd = '/var/local/bin/dbmpasswd';
my $dbmgroup = '/var/local/bin/dbmgroup';
my $drmpasswdf = '/var/local/spmz/spamhippo/db/passwd';
my $drmgroupf = '/var/local/spmz/spamhippo/db/group';

my $logfile = "/var/log/adduser";
my $webhostlog = "/var/log/webhostlog";
my $pwd_mkdb = "env PW_SCAN_BIG_IDS=1 /usr/sbin/pwd_mkdb -p";
my $etc_passwd = "/etc/master.passwd";
my $plock = "/etc/plock";
my $ptmp = "/etc/ptmp";

my $user_id_begin = "16000";
my $user_id_end = "19999";
my $skip_id_begin = "29900";
my $skip_id_end = "31000";
my $user_id_max = "19999";

	my $delmcontent;
	my ($userid, $password, $fullname);
	if (($ARGV[0] =~ /^-/) && (@ARGV == 2))
	{
		($delmcontent, $userid) = @ARGV;
	}
	else
	{
		($userid, $password, $fullname) = @ARGV;
		die "usage: chgwhostpass userid passwd fullname\n"
			if $userid eq '' || $password eq '';
	}

	unless($userid =~ /^[_A-Za-z0-9]+$/)
	{
		print "Username can only be alphanumeric\n";
		exit 1;
	}

	if ($delmcontent ne '')
	{
		system("set -x; rm -f /var/mail/.$userid.pop /var/mail/$userid /tmp/mail/.$userid.pop; cd /var/local/mail; rm -r -f $userid");
		exit(0);
	}

	my $maxsleep = 150;		# can only sleep for 5 minutes
	my $count = 0;
	while(1)
	{
		if (LockFile($plock) != 0)
		{
			last;
		}
		if($count == $maxsleep)
		{
			MailAdmin("chgwhostpass.pl",
				["cannot lock $plock"]);
			die "chgwhostpass.pl: cannot lock $plock for $userid\n";
		}
		$count++;
		sleep 2;
	}

	open(PASSWD, $etc_passwd) ||
		die "can't open $etc_passwd\n";
	umask 077;
	open(PTMP, "> $ptmp") ||
		die "can't write new passwd file!\n";

	my $c_uid = 0;
	my $u_id = $user_id_begin;
	my $change = 0;
	my $cryptpwd = "";
	my $pwname;
	$cryptpwd = crypt($password, &salt) if $password ne "";
	my ($endcolon, $p_uid);
	while(<PASSWD>)
	{
		chop;
		$endcolon = /:$/ ? ":" : "";
	
		my @fields = split(/:/);
		if($fields[0] eq $userid)
		{
			$fields[1] = $cryptpwd;
			$fields[7] = $fullname if $fullname ne '';
			$pwname = $fields[7];
			$change = 1;
			$c_uid = $fields[2];
		}
		$p_uid = $fields[2];
		print PTMP join(':',@fields), "$endcolon\n";
		next if($p_uid > $user_id_max);
		$u_id = $p_uid+1 if(($p_uid < $user_id_end ||
			$p_uid >= $skip_id_end) && $u_id <= $p_uid);
		$u_id = $skip_id_end if($u_id >= $skip_id_begin &&
			$u_id < $skip_id_end);
	}

	close PASSWD;
	close PTMP;
	if($change)
	{
		rename($etc_passwd, $etc_passwd . ".old");
		chmod 0600, $ptmp;
		rename($ptmp, $etc_passwd);
		system("$pwd_mkdb -u $userid $etc_passwd");
		&append_file($webhostlog,
			"$userid:*:$c_uid:*:$pwname change password");
		system("
			$dbmpasswd -d $drmpasswdf $userid
			$dbmpasswd $drmpasswdf $userid $password
			$dbmgroup adduser $drmgroupf $userid mail
		");
	}
	elsif ($fullname ne '')
	{
		unlink $ptmp;
		my $g_id = 1100;

		my $home = "/var/ftp/./pub";

		my $new_entry = "$userid\:" . "$cryptpwd" .
			"\:$u_id\:$g_id\::0:0:$fullname:$home:$sh";

		&append_file($etc_passwd, $new_entry);
		system("$pwd_mkdb -u $userid $etc_passwd");
		system("rsh -n $mx_server /usr/local/sbin/adduserpwd '$userid' '$g_id' '\"$fullname\"' '$password' $u_id") if $mx_server ne "";
		&append_file($webhostlog, "$new_entry");
    		&adduser_log("$userid:*:$u_id:$g_id:$fullname");
		system("
			$dbmpasswd -d $drmpasswdf $userid
			$dbmpasswd $drmpasswdf $userid $password
			$dbmgroup adduser $drmgroupf $userid mail
		");
	}

	UnlockFile($plock);
	exit 0;

sub salt {
	my($salt);		# initialization
	my($i, $rand);
	my(@itoa64) = ( 0 .. 63 ); # 0 .. 63

	# to64
	for ($i = 0; $i < 8; $i++) {
		srand(time + $rand + $$); 
		$rand = rand(25*29*17 + $rand);
		$salt .=  $itoa64[$rand & $#itoa64];
	}

	return '$1$' . $salt;
}

sub append_file {
	my($file,@list) = @_;
	my($e);
	my($LOCK_EX) = 2;
	my($LOCK_NB) = 4;
	my($LOCK_UN) = 8;

	open(F, ">> $file") || die "$file: $!\n";
	my $n = 0;
	while(!flock(F, $LOCK_EX | $LOCK_NB)) {
		die "Cannot lock file $file" if(++$n > 100);
		sleep(1);
	}
	print F join("\n", @list) . "\n";
	close F;
	flock(F, $LOCK_UN);
}

sub adduser_log {
	my($string) = @_;
	my($e);

	return 1 if $logfile eq "no";

	my($sec, $min, $hour, $mday, $mon, $year) = localtime;
	$mon++;

	foreach $e ('sec', 'min', 'hour', 'mday', 'mon', 'year') {
		# '7' -> '07'
		eval "\$$e = 0 . \$$e" if (eval "\$$e" < 10);
	}

	&append_file($logfile, "$year/$mon/$mday $hour:$min:$sec $string");
}
