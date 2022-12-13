package drmsettings;

use vars qw(@ISA @EXPORT);
use drmdef;

use vars qw($SettingsDir);

use Exporter;
@ISA = qw(Exporter);

@EXPORT =
   qw(
	$folderframe
	$previewframe
	$listfontsize
	&GetSettings
	&FetchSettings
   );

$folderframe = 1;
$previewframe = 1;
$listfontsize = 'xx-small';

sub FetchSettings
{
	my $folderframe = 1;
	my $previewframe = 1;
	my $listfontsize = 'xx-small';
	my $checktime = 0;

	my $who = $ENV{REMOTE_USER};
	my $prefname = $SettingsDir;
	$prefname .= $who;
	$prefname .= "/.fromrc";

	if (open(PREF, "<$prefname"))
	{
		while(<PREF>)
		{
			chop;
			my($header, $value) = split(/\s+/);
			if ($header eq 'folderframe')
			{
				$folderframe = ($value ne 'off');
			}
			elsif ($header eq 'previewframe')
			{
				$previewframe = ($value ne 'off');
			}
			elsif ($header eq 'listfontsize')
			{
				$listfontsize = $value;
			}
			elsif ($header eq 'checktime')
			{
				$checktime = 0+$value;
			}
		}
	}

	return ($folderframe, $previewframe, $listfontsize,
		$checktime);
}

sub GetSettings
{
	($folderframe, $previewframe, $listfontsize) =
		FetchSettings();
}

1;
