<%
use lib qw(../../drm-bin);
use drmdef;
my $title = "Direct Read Mail Help";
%>
<html>
<head>
<title></title>
<link rel="stylesheet" type="text/css" href="<%=$CSSDIR%>drmmenu.css" />
</head>
<body style="font-family: Arial; font-size: 90%">
<div "style=position: relative">
<div id="stitleBar"><img src="<%=$IMGDIR%>toolbartop.gif" height="28" width="100%"></div><div id="mainMenu" class="mainMenu" style="min-width: 720px; background-image:url('<%=$IMGDIR%>menubg.gif');">
  <div style="margin:0; padding: 4px 15px 0 80px;">
    <ul id="menuList1" class="menuList">
      <li>&nbsp;</li>
    </ul>
  </div>
</div>
<div style="position:absolute;top:0;left:10px;"><a href="${HOMEPAGE}"><img id="stitleImage" src="<%=$IMGDIR%>stitlelogo.gif" border=0></a></div>
</div>
<div align="left">
  <table border="0" cellpadding="0" cellspacing="0" style="border-collapse: collapse" bordercolor="#111111" width="1000">
    <tr>
      <td bgcolor="#FFFFFF">
      <div align="left">
        <table border="0" cellpadding="0" cellspacing="0" style="border-collapse: collapse" bordercolor="#111111" width="1010" align="left">
          <tr>
            <td bgcolor="#EAEFF8" height="25">
      <font face="Arial" size="2" color="#396BB5"><b>&nbsp;<%=$title%></b></font></td>
          </tr>
          <tr>
            <td bgcolor="#396BB5" width="6">
            <img border="0" src="<%=$IMGDIR%>spacer_2pixel.gif" width="5" height="2"></td>
          </tr>
        </table>
      </div>
      </td>
    </tr>
  </table>
</body>
<html>
