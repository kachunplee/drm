 <div id="titleMsg"><% if ($MEMDRIVEURL ne '') { %>
  <a href="javascript:OpenMembershipDriveWin('<%=$MEMDRIVEURL%>')"><img border=0 align=middle border=0 src=/drm/img/membership_drive3.gif width=140 height=24></a><% } %></div>
 <div id="titleBar" style="min-width: 720px;"><img src="/drm/img/toolbartop.gif" height="28" width="100%"></div>
 <div id="mainMenu" class="mainMenu" style="min-width: 720px; background-image:url('/drm/img/menubg.gif');">
  <div style="margin:0; padding: 4px 15px 0 120px;">
    <ul id="menuList1" class="menuList">
            <li>
              <a class="selector" href="javascript:dspFolder('in')"><b>Check Mail</b></a>
            </li>
            | <li>
     	      <a class="selector" href="javascript:OpenComposeWin('<%=$BINDIR%>compose')"><b>New Message</b></a>
            </li>
            | <li id="Option">
              <a id="EditSelector" class="selector"><b>Edit</b></a>
              <ul id="EditMenu" class="menu">
                <li><a href="javascript:drmDrag.selectAll()">Select All</a></li>
                <li><a href="javascript:MarkReadSelected()">Mark Read</a></li>
                <li><a href="javascript:MarkUnreadSelected()">Mark Unread</a></li>
                <li><a href="javascript:OpenPrefWin('<%=$BINDIR%>drmpref')">Preferences</a></li>
                <li><a href="javascript:OpenPrefWin('<%=$BINDIR%>prefspell')">Dictionary</a></li>
              </ul>
            </li>
            | <li>
              <a class="selector" href="/acc/user_global.asp"><b>SpamHippo</b></a>
            </li>
            | <li>
              <a class="selector" href="javascript:OpenAddBookWin('<%=$BINDIR%>drmaddbk')"><b>Address Book</b></a>
            </li>
            | <li>
              <a class="selector" href="javascript:OpenHelpWin('<%=$HTDIR%>drmhelp.asp')"><b>Help</b></a>
            | <li>
              <a class="selector" href="/cgi-bin/ulogout"><b>Logout</b></a>
    </ul>
  </div>
 </div>
 <div style="position:absolute;top:0;left:10px;"><a href="<%=$HOMEPAGE%>"><img id ="titleImage" src="/drm/img/titlelogo.gif" border=0></a></div>
<script language=javascript>
<!--
	initializeMenu("EditMenu", "EditSelector", 0, 0);
//-->
</script>
