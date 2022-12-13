<%
$Response->{Expires} = 0; $Response->{CacheControl} = 'no-cache';

my $env = $Request->ServerVariables;

use strict;

use lib qw(/var/local/libdata/perl5);

no strict;
<!--#include file=../../drm-bin/drmpref.ph-->

use strict;

use vars qw($MAILHOME);

my $user = $env->{REMOTE_USER};

my $aliasrc = "$MAILHOME/$user/.aliasrc";
my %RC;
ReadRC($aliasrc, \%RC, 1);

my @a;
my @b;
foreach my $k (sort keys %RC)
{
	push @a, $k;
	push @b, $RC{$k};
}

my $q=uc($Request->QueryString("q"));

# lookup all hints from array if length of q>0
my @hint;
my @addr;
my $n = length($q);
if ($n>0)
{
  if ($q eq '*')
  {
    for (my $i = 0; $i < scalar(@a); $i++)
    {
  	push @hint, $Server->HTMLEncode($a[$i]);
  	push @addr, $Server->HTMLEncode($b[$i]);
    }
  }
  else
  {
    for (my $i = 0; $i < scalar(@a); $i++)
    {
      if ($q eq uc(substr($a[$i],0,$n)))
      {
  	push @hint, $Server->HTMLEncode($a[$i]);
  	push @addr, $Server->HTMLEncode($b[$i]);
      }
    }
  }
}

my $res = ",,no,,";
if (0 < @hint)
{
	$res = join(', ', @hint) . "\n" . join(', ', @addr);
}
$Response->write($res);
%>
