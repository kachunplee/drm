<%
use lib qw(../../drm-bin);
use drmdef;
%>
<frameset rows=90,* framespacing=0 frameborder=0 border=1>
  <frame name=DRMHelpHeader marginheight=0 marginwidth=0 scrolling=no NORESIZE src="<%=$HTDIR%>helpheader.asp">
  <frame name=DRMHelpBody marginheight=5 marginwidth=5 src="<%=$HTDIR%>helpmain.html">
</frameset>
