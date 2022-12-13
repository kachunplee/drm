<%
$Response->{Expires} = 0; $Response->{CacheControl} = 'no-cache';

use strict;

my $q=$Request->QueryString("q");

my $zoneinfo = '/usr/share/zoneinfo/' . $q;

my @tz;
if (opendir(ZDIR, $zoneinfo))
{
	while(my $f = readdir ZDIR)
	{
		next if $f =~ /[.]/;
		push @tz, $f;
	}
	close ZDIR;
}

$Response->write((0==@tz) ? "//" : join("\n", sort @tz));
%>
