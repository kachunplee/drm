#! /usr/bin/perl

$|=1;

$MAILHOME = "/var/local/mail";
$user = $ARGV[0];
shift;
$srcFile = $ARGV[0];
$destFile = $ARGV[0] . ".a";
$userDir = "$MAILHOME/$user";
$aliasrc = "$MAILHOME/$user/.aliasrc";

undef %ALIASES;
if(-f $aliasrc)
{
	if(!open(RC, $aliasrc)) {
		exit 0;
	}
	while(<RC>)
	{
		chop;
		($name, $email) = split(':\s', $_);
		next if($name eq "");
		$ALIASES{$name} = $email;
	}
	close RC;
}

if(open(RC, $srcFile))
{
	undef %RECIPS;
	while(<RC>)
	{
		chop;
		if($_ eq "")
		{
			# end of headers
			last;
		}
		if(/^(To|Bcc|Cc): /)
		{
			$_ =~ s/^.*:\s+//g;
			@emails = split(',', $_);
			for($i = 0; $i < @emails; $i++)
			{
				if(defined($ALIASES{$emails[$i]}))
				{
					$RECIPS{$ALIASES{$emails[$i]}} = 1;
				}
				else
				{
					$RECIPS{$emails[$i]} = 1;
				}
			}
		}
	}
	close RC;

	$bExpand = 1;
	while($bExpand == 1)
	{
		$bExpand = 0;
		foreach $key (keys %RECIPS)
		{
			if($RECIPS{$key} == 0)
			{
				next;
			}
			@emails = split(',', $key);
			$count = @emails;
			if($count > 1)
			{
				$RECIPS{$key} = 0;
				for($i = 0; $i < @emails; $i++)
				{
					if(defined($ALIASES{$emails[$i]}))
					{
						$RECIPS{$ALIASES{$emails[$i]}} = 1;
					}
					else
					{
						$RECIPS{$emails[$i]} = 1;
					}
				}
				$bExpand = 1;
				last;
			}
		}
	}

	$cnt = 0;
	if(open(RC, ">$destFile"))
	{
		foreach $key (keys %RECIPS)
		{
			if($RECIPS{$key} == 1)
			{
				$key =~ s/'/'"'"'/g;
				if ($cnt > 0)
				{
					print RC ", $key";
				}
				else
				{
					print RC "$key";
				}
				$cnt++;
			}
		}
		close RC;
	}
}

exit 0;
