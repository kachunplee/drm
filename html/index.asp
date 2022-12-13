<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<!--#include file="drmdef.inc"-->
<%
  use drmdef;
  use drmsettings;

  my $env = $Request->ServerVariables;
  my $qry = $Request->QueryString;

  my ($folderframe, $previewframe, $listfontsize, $checktime)
	= FetchSettings();
%>
<title><%=$SERVICENAME%>'s Direct Read Mail (tm)</title>
<link rel="stylesheet" type="text/css" href="/drm/css/drmlayout.css" />
<link rel="stylesheet" type="text/css" href="/drm/css/drmmenu.css" />
<link rel="stylesheet" type="text/css" href="/drm/css/drm.css" />

<script type="text/javascript" src="/drm/js/Helpers.js"></script>
<script type="text/javascript" src="/drm/js/NumberHelper.js"></script>
<script type="text/javascript" src="/drm/js/StringHelper.js"></script>
<script type="text/javascript" src="/drm/js/HTTP.js"></script>
<script type="text/javascript" src="/drm/js/Ajax.js"></script>
<script type="text/javascript" src="/drm/js/ajax.js"></script>

<script type="text/javascript">
  var previewframe = <%=$previewframe?'true':'false'%>;
  var fontsize = '<%=$listfontsize%>';
  var checktime = <%=$checktime%>;
<%
  #
  # setup Folder variables
  #
  $Response->write(`cd ../../drm-bin; ./drmfolder $qry->{folder}`);
%>

</script>

<script type="text/javascript" src="/drm/js/drmfun.js"></script>
<script type="text/javascript" src="/drm/js/drmdrag.js"></script>
<script type="text/javascript" src="/drm/js/drmlist.js"></script>
<script type="text/javascript" src="/drm/js/drmwinadj.js"></script>
<script type="text/javascript" src="/drm/js/drmresize.js"></script>
<script type="text/javascript" src="/drm/js/drmwin.js"></script>
<script type="text/javascript" src="/drm/js/drmfloat.js"></script>
<script type="text/javascript" src="/drm/js/drmfolder.js"></script>
<script type="text/javascript" src="/drm/js/dropmenu.js"></script>

<script type="text/javascript">

function initialize()
{
	var ele = document.getElementById("drm_right");
	disableSelection(ele);

	displayList(currentFolder);
	initPreview();
	InitMenu();
}
</script>
</head>

<body onload="javascript:initialize();">
<!--
  Progress panel
-->
<div id="loading" style="display: none;"></div>
<!--
  DRM Top Menu
-->
<div id="drm_menu">
<!--#include file=drmmenu.inc-->
</div>
<div id="drm_display">
<!--
  DRM Folder
-->
<div id="drm_folder">
<!--#include file=drmfolder.inc-->
</div>
<!--
  DRM List
-->
<div id="drm_right">
 <div id="drm_list">
  <div style="width: 100%">
  <div id="drm_list_user"><b>You are signed in as:</b> <%=$env->{REMOTE_USER}%></div>
  <div id="drm_list_title" class="drm_title">In Box</div></div>
  <div id="drm_list_hdr" class="drm_row_odd">
    <div class="drm_col_s">Status</div>
    <div class="drm_col_0"><span id="date_sort_span" style="position: relative; margin: 0; padding: 0;"><img align=top src="/drm/img/blue_arrowup.gif" border="0" width="10" height="15"></span><a href="javascript:sortFolder('d')">Date</a></div>
    <div class="drm_col_1"><span id="from_sort_span" style="position: relative; margin: 0; padding: 0;"></span><a href="javascript:sortFolder('w')"><span id="who_col_span">From</span></a></div>
    <div class="drm_col_2"><span id="subject_sort_span" style="position: relative; margin: 0; padding: 0;"></span><a href="javascript:sortFolder('s')">Subject</a></div>
  </div>
  <div id="drm_list_dsp"></div>
  <div id="drm_list_mask"><div id="drm_mask_msg">Please wait...</div></div>
 </div><% if ($previewframe) { %>
<!--
  DRM Preview
-->
 <!--div id="drm_sep_div" onmousedown="sepMouseClick()"></div-->
 <div id="drm_sep_div"></div>
 <div id="drm_prevw">
  <div id="drm_prevw_hdr">
    <table border=0 cellpadding=0 cellspacing=0 width=100%>
      <tr>
	<td height=17 valign=top width=69><b>Subject:</b></td>
	<td height=17 valign=top colspan=2 class="drm_prevw_hdr_f" id="drm_prevw_subj"></td>
      </tr>
      <tr>
	<td height=17 valign=top width=69><b><span id="who_prevw_span">From</span>:</b></td>
        <td height=17 valign=top class="drm_prevw_hdr_f" id="drm_prevw_from"></td>
        <td height=17 valign=top align=right class="drm_prevw_hdr_f" id="drm_prevw_print"></td>
      </tr>
    </table>
  </div>
  <div id="drm_prevw_dsp"><iframe id="drmPreview" height=300 frameborder=0 src="drmprevw.asp" name="drmPreview" scrolling="yes" onload="initPreview"></iframe>
  </div>
 </div>
<% } else { %><script>
	resizeFrame();
</script>
<% } %></div>
</div>
</body>
</html>
