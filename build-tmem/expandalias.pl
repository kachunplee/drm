#! /usr/bin/perl

$|=1;

$MAILHOME = "/var/local/mail";
$user = $ARGV[0];
shift;
$srcFile = $ARGV[0];
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
		($name, $email) = split(/:\s*/, $_);
		next if($name eq "");
		$ALIASES{$name} = $email;
	}
	close RC;
}

if(open(RC, $srcFile))
{
	$sNew = 0;
	if (open(TRC, ">$srcFile.new"))
	{
		$sNew = 1;
	}
	undef %RECIPS;
	while(<RC>)
	{
		chop;
		if($_ eq "")
		{
			# end of headers
			if ($sNew == 1)
			{
				print TRC $_, "\n"; 
			}
			last;
		}
		if(/^(To|Bcc|Cc): /)
		{
			/(^.*:\s+)(.*)/;
			$hdr = $1;
			@emails = split(/,\s*/, $2);
			$em = '';
			for($i = 0; $i < @emails; $i++)
			{
				if ($em ne '')
				{
					$em .= ", ";
				}
				if(defined($ALIASES{$emails[$i]}))
				{
					$RECIPS{$ALIASES{$emails[$i]}} = 1;
					$em .= $ALIASES{$emails[$i]};
				}
				else
				{
					$RECIPS{$emails[$i]} = 1;
					$em .= $emails[$i];
				}
			}
			if ($sNew == 1)
			{
				print TRC $hdr, $em, "\n";
			}
		}
		elsif ($sNew == 1)
		{
			print TRC $_, "\n"; 
		}
	}
	if ($sNew == 1)
	{
		while(<RC>)
		{
			print TRC $_;
		}
	}
	close RC;

	if ($sNew == 1)
	{
		close TRC;
		rename("$srcFile.new", $srcFile);
	}

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
			@emails = split(/,\s*/, $key);
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
}

exit 0;
