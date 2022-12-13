$MAILHOME="/var/local/mail";

use strict;

sub GetForm
{
	my ($form) = @_;
	my ($inbuf, $pair, $action);
	read (STDIN, $inbuf, $ENV{'CONTENT_LENGTH'});
	my @pairs = split(/&/, $inbuf); 
	foreach $pair (@pairs)
	{
		my ($name, $value) = split(/=/, $pair);
		$value =~ tr/\+/ /;
		$value =~ s/%([a-fA-F0-9][a-fA-F0-9])/pack("C", hex($1))/eg;

		if ($name =~ /^(action_[^.]*)\./)
		{
			$action = $1;
			next;
		}

		if ($name eq "aliasemail")
		{
			$value =~ s/[\r\n]/\,/g;
			$value =~ s/\s+//g;
			$value =~ s/\,+/\,/g;
			$value =~ s/\,+$//;
		}

		$form->{$name} = $value;
	}

	return $action;
}

#
#
#
sub ReadRC
{
	my ($rc, $prc, $skipColon) = @_;
	return 0 unless (open(RC, $rc));

	my $mtime =  (stat RC)[9];

	while(<RC>)
	{
		chop;
		if (/^([^\t]*)\t(.*)$/)
		{
			my $nickname = $1;
			my $email = $2;
			if ($skipColon == 1)
			{
				$nickname =~ s/:$//;
			}
			$prc->{$nickname} = $email;
		}
	}

	close RC;
	return $mtime;
}

sub IsRCChg
{
	my ($prc, $pdata) = @_;
	my $nc = 0;
	foreach my $fd (keys %$pdata)
	{
		if ($pdata->{$fd} ne $prc->{$fd})
		{
			$prc->{$fd} = $pdata->{$fd};
			++$nc;
		}
	}

	return $nc;
}

sub SaveRC
{
	my ($userDir, $rc, $prc, $delete) = @_;

	mkdir($userDir, 0775) if(! -d $userDir);

	my ($tmpsrc) = $rc . '.tmp';
	if(open(DRC, ">$tmpsrc"))
	{
		foreach my $fd (keys %$prc)
		{
			next if ($fd =~ /^x_/);
			my $data = $prc->{$fd};
			if (($delete != 1)  || ($data ne ''))
			{
				print DRC $fd, "\t", $prc->{$fd}, "\n";
			}
		}
		close DRC;
		rename($tmpsrc, $rc);
	}
}

#
#
#
sub ParseFrom
{
	return split('\t', $_[0]);
}

sub ParseChecktime
{
	# check for all digit
	return 0 + $_[0];
}

sub ParseSoundsrc
{
	return $_[0];
}

sub ParseFolderFrame
{
	my ($name) = split('\t', $_[0]);
	return ($name eq "on") ? "checked" : "";
}

sub ParsePreviewFrame
{
	my ($name) = split('\t', $_[0]);
	return ($name eq "on") ? "" : "checked";
}

sub ParseDspImage
{
	my ($name) = split('\t', $_[0]);
	return ($name eq "on") ? "checked" : "";
}

sub ParseListFontSize
{
	return $_[0];
}

sub ParseTZName
{
	return $_[0];
}

sub ParseDateFormat
{
	return $_[0];
}
