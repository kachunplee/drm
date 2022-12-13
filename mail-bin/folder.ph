#
# add line to file
#
sub f_add_line
{
	my ($file, $line) = @_;

	my $tmpfile = $file . ".$$";

	open(ORIGFILE, $file);
	open(OUTFILE, ">$tmpfile") || die("Cannot open $tmpfile\n");
	flock(ORIGFILE, 2);

	my $cmp = -1;
	my $find = 0;
	my $of;
	while (<ORIGFILE>)
	{
		chomp;
		$of = $_;
		if ($cmp < 0)
		{
			s/[\+-]$//;
			$cmp = ($_ cmp $line);
			if ($cmp == 0)
			{
				$find = 1;
				last;
			}
			print OUTFILE $line, "\n" if ($cmp > 0);
		}

		print OUTFILE $of, "\n";
	}

	print OUTFILE $line, "\n" if ($cmp < 0);

	close OUTFILE;
	close ORIGFILE;

	if ($find)
	{
		unlink($tmpfile);
	}
	else
	{
		rename($tmpfile, $file);
	}
}

#
# delete line
#
sub f_del_line
{
	my ($file, $line) = @_;

	my $tmpfile = $file . ".$$";
	my $find;

	open(ORIGFILE, $file) || die("Cannot open $file\n");
	open(OUTFILE, ">$tmpfile") || die("Cannot open $tmpfile\n");
	flock(ORIGFILE, 2);

	my $of;
	while (<ORIGFILE>)
	{
		chomp;
		$of = $_;
		s/[\+-]$//;
		if($_ eq $line)
		{
			$find = 1;
		}
		else
		{
			print OUTFILE $of, "\n";
		}
	}

	close OUTFILE;
	close ORIGFILE;

	if ($find)
	{
		rename($tmpfile, $file);
	}
	else
	{
		unlink($tmpfile);
	}
}

sub f_is_buildin
{
	my ($foldername) = @_;
	$foldername =~ tr/A-Z/a-z/;
	return ($foldername eq 'in' || $foldername eq 'out' ||
		$foldername eq 'trash');
}

sub f_is_exist
{
	my ($file, $foldername) = @_;
	$foldername =~ tr/A-Z/a-z/;

	if (! -e $file)
	{
		return 0;
	}
	open(ORIGFILE, $file) || die("Cannot open $file\n");
	flock(ORIGFILE, 2);

	my $find = 1;
	my $num = 0;
	while (<ORIGFILE>)
	{
		chomp;
		++$num;
		if ($_ eq $foldername)
		{
			$find = -1;
			last;
		}
	}

	close ORIGFILE;
	return $find * $num;
}

sub d_is_exist
{
	my ($file, $foldername) = @_;
	$foldername =~ tr/A-Z/a-z/;

	if (! -e $file)
	{
		return 0;
	}
	open(ORIGFILE, $file) || die("Cannot open $file\n");
	flock(ORIGFILE, 2);

	my $find = 1;
	my $num = 0;
	my $dirname = $foldername . '/';
	while (<ORIGFILE>)
	{
		chomp;
		++$num;
		s/[\+-]$//;
		if ($_ eq $foldername || $_ eq $dirname)
		{
			$find = -1;
			last;
		}
	}

	close ORIGFILE;
	return $find * $num;
}

sub addfolder
{
	my ($user, $folderpath, $foldername, $bskipexistcheck) = @_;
	my $dir = "/var/local/mail/$user";
	mkdir($dir, 0775) if(! -d $dir);

	$foldername =~ tr/A-Z/a-z/;
	if (f_is_buildin($foldername))
	{
		return "You requested a name that is already in use.";
	}

	if (length($foldername) < 3 || length($foldername) > 10)
	{
		return "Folder name must be 3 to 10 characters long.\n";
	}

	if ($foldername =~ /[^a-z0-9]/)
	{
		return "Folder names must use alphanumeric characters.\n";
	}

	my $folderfile = "/var/local/mail/$user/Folders";
	my $fname = $folderpath . $foldername;

	my $num_folder = f_is_exist($folderfile, $fname);
	if ($num_folder < 0)
	{
		if ($bskipexistcheck)
		{
			return '';
		}
		return "You requested a name that is already in use.\n";
	}

	if ($num_folder >= $MAXFOLDER)
	{
		return "This mailbox has reached the maximum number of folders.\n";
	}

	f_add_line($folderfile, $fname);

	my $folderdir = "$dir/mail";
	system("mkdir -p 0775 $folderdir") if(! -d $folderdir);
	return '';
}

sub addfolderdir
{
	my ($user, $folderpath, $foldername, $bskipexistcheck) = @_;
	my $dir = "/var/local/mail/$user";
	mkdir($dir, 0775) if(! -d $dir);

print STDERR "user=$user, folderpath=$folderpath, foldername=$foldername\n";

	$foldername =~ tr/A-Z/a-z/;
	if ($folderpath eq '' && f_is_buildin($foldername))
	{
		return "You requested a name that is already in use.";
	}

	if (length($foldername) < 3 || length($foldername) > 10)
	{
		return "Directory name must be 3 to 10 characters long.\n";
	}

	if ($foldername =~ /[^a-z0-9]/)
	{
		return "Directory names must use alphanumeric characters.\n";
	}

	my $folderfile = "/var/local/mail/$user/Folders";
	my $folderdir = $folderpath . $foldername;

	my $num_folder = d_is_exist($folderfile, $folderdir);
	if ($num_folder < 0)
	{
		if ($bskipexistcheck)
		{
			return '';
		}
		return "You requested a name that is already in use.\n";
	}
	my $fdir = "$dir/mail/" . $folderdir;
	$folderdir .= '/';

	f_add_line($folderfile, $folderdir);

	system("mkdir -p 0775 $fdir") if(! -d $fdir);
	return '';
}

sub readfolders
{
	my ($user) = @_;
	my $filename = "/var/local/mail/$user/Folders";
	my $userfolder = 0;
	my @folder;
	if (open(FOLDERS, $filename))
	{
		while(<FOLDERS>)
		{
			++$userfolder;
			chomp;
			s/\s//g;
			push(@folder, $_);
		}
		close FOLDERS;
	}
	return ($userfolder, @folder);
}

1;
