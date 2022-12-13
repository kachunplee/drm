sub GetDictDataDir
{
	my ($MASTERID) = @_;

	return "/var/local/data/$MASTERID/";
}

sub CheckDictionary
{
	my ($localdir, $localdict, $dictserver, $dctf) = @_;

	return 0 if $dictserver eq '';

	my $mtime = (stat "$localdict.per") [9] + 0;
	return 0 if not open(DICT, "rsh -t 30 -n $dictserver /usr/local/sbin/checkdict $dctf $mtime |");

	my $status = <DICT>;
	if ($status !~ /^DICT NEW: (\d+)/)
	{
		close DICT;
		return 0;
	}

	my $r_mtime = $1;

	print STDERR 'drmspell: ', $dictserver, ': DICT NEW: ',
		$r_mtime, ' <> ', $mtime, "\n";

	if (open(UPDATE, "| (set -x; /usr/local/sbin/updatedict $localdir $localdict $r_mtime )"))
	{
		while(<DICT>)
		{
			print UPDATE $_;
		}
	}

	# Get new dictionary
	close DICT;
}

1;
