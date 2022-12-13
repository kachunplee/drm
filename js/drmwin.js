var DRMMsgWin = null;
var DRMPrnMsgWin = null;
var DRMComposeWin = null;
var DRMPrefWin = null;
var DRMFilterWin = null;
var DRMAddBookWin = null;
var DRMVfyAddrWin = null;
var DRMHelpWin = null;

function OpenMsgWin (winurl)
{
	if (DRMMsgWin && DRMMsgWin.closed) delete DRMMsgWin;

	DRMMsgWin = window.open(winurl, "DRMMsgWin",
		"width=650,height=500,resizable=yes,scrollbars=yes,toolbar=no,status=yes");
	DRMMsgWin.focus();
}

function OpenPrnMsgWin (winurl)
{
	if (DRMPrnMsgWin && DRMPrnMsgWin.closed) delete DRMPrnMsgWin;

	DRMPrnMsgWin = window.open(winurl, "DRMPrnMsgWin",
		"width=650,height=500,resizable=yes,scrollbars=yes,toolbar=no,status=yes");
	DRMPrnMsgWin.focus();
}

function OpenComposeWin (winurl)
{
	if ((DRMComposeWin != null) && !DRMComposeWin.closed)
		DRMComposeWin.location.href = winurl;
	else
	{
		var winname = "DRMComposeWin";
		DRMComposeWin = window.open(winurl, winname,
			"width=700,height=590,resizable=no,scrollbars=yes,toolbar=no,status=yes");
	}

	DRMComposeWin.focus();
}

function OpenPrefWin (winurl)
{
	if ((DRMPrefWin != null) && !DRMPrefWin.closed)
		DRMPrefWin.location.href = winurl;
	else
	{
		var winname = "DRMPrefWin";
		DRMPrefWin = window.open(winurl, winname,
			"width=680,height=720,resizable=no,scrollbars=yes,toolbar=no,status=yes");
	}

	DRMPrefWin.focus();
}

function OpenFilterWin (winurl)
{
	if ((DRMFilterWin != null) && !DRMFilterWin.closed)
		DRMFilterWin.location.href = winurl;
	else
	{
		var winname = "DRMFilterWin";
		DRMFilterWin = window.open(winurl, winname,
			"width=680,height=540,resizable=no,scrollbars=yes,toolbar=no,status=yes");
	}

	DRMFilterWin.focus();
}

function OpenAddBookWin (winurl)
{
	if ((DRMAddBookWin != null) && !DRMAddBookWin.closed)
		DRMAddBookWin.location.href = winurl;
	else
	{
		var winname = "DRMAddBookWin";
		DRMAddBookWin = window.open(winurl, winname,
			"width=680,height=520,resizable=no,scrollbars=yes,toolbar=no,status=yes");
	}

	DRMAddBookWin.focus();
}

function OpenVfyAddrWin (winurl)
{
	if ((DRMVfyAddrWin != null) && !DRMVfyAddrWin.closed)
		DRMVfyAddrWin.location.href = winurl;
	else
	{
		var winname = "DRMVfyAddrWin";
		DRMVfyAddrWin = window.open(winurl, winname,
			"width=680,height=520,resizable=no,scrollbars=yes,toolbar=no,status=yes");
	}

	DRMVfyAddrWin.focus();
}

function OpenHelpWin (winurl)
{
	if ((DRMHelpWin != null) && !DRMHelpWin.closed)
		DRMHelpWin.location.href = winurl;
	else
	{
		var winname = "DRMHelpWin";
		DRMHelpWin = window.open(winurl, winname,
			"width=680,height=520,resizable=no,scrollbars=no,toolbar=no,status=yes");
	}

	DRMHelpWin.focus();
}

function OpenMembershipDriveWin(memdriveurl)
{
        var MembershipDriveWin = window.open(memdriveurl, "MembershipDrive", "resizable=yes,scrollbars=yes,status=0,width=460,height=450");
        MembershipDriveWin.focus();
}

function OpenErrorWin (message)
{
	if(message == "mailto" || message == "from")
	{
		var printmessage = 'One or more required fields are empty.';
		var winname = "DRMSendError";
	}
	if(message == "forwardfilter")
	{
		var printmessage = "A forwarding email address must be specified if you've selected the &quot;Forward to&quot; filter action option.";
		var winname = "DRMFilterError";
	}
	if(message == "maxbox")
	{
		var printmessage = 'This mailbox has reached the maximum number of folders.';
		var winname = "DRMMaxboxError";
	}
	if(message == "folderdir")
	{
		var printmessage = 'This folder directory is not empty and cannot be deleted.';
		var winname = "DRMError";
	}
	if(message == "")	
	{
		var printmessage = 'Unable to process your request at this time.';
		var winname = "DRMError";
	}


	DRMErrorWin = window.open("", winname, "width=300,height=200,resizable=no,scrollbars=no,toolbar=no,status=yes");

	var d = DRMErrorWin.document;


d.writeln ('<head>\n');
d.writeln ('<title></title>');
d.writeln ('</head>');

d.writeln ('<body link="#326698" vlink="#326698" alink="#326698" bgcolor="#EAEFF8" marginheight="0" marginwidth="0" topmargin="0" leftmargin="0">');
d.writeln ('  <div align="center">');
d.writeln ('    <table border="0" cellpadding="0" cellspacing="0" style="border-collapse: collapse" bordercolor="#111111" width="300">');
d.writeln ('        <tr>');
d.writeln ('        <td bgcolor="#214A84"><img border="0" src="/drm/logo2_popup.gif" width="134" height="29"></td>');
d.writeln ('      </tr>');
d.writeln ('      <tr>');
d.writeln ('        <td bgcolor="#EAEFF8">');
d.writeln ('      <img border="0" src="/drm/spacer_transparent.gif" width="25" height="7"></td>');
d.writeln ('      </tr>');
d.writeln ('      <tr>');
d.writeln ('        <td bgcolor="#EAEFF8">');
d.writeln ('      <div align="center">');
d.writeln ('        <center>');
d.writeln ('        <table border="0" cellpadding="0" cellspacing="0" style="border-collapse: collapse" bordercolor="#111111" width="275">');
d.writeln ('          <tr>');
d.writeln ('            <td width="256">');
d.writeln ('      <img border="0" src="/drm/spacer_transparent.gif" width="25" height="7"></td>');
d.writeln ('         </tr>');
d.writeln ('         <tr>');
d.writeln ('            <td width="256">');
d.writeln ('      <img border="0" src="/drm/spacer_transparent.gif" width="25" height="7"></td>');
d.writeln ('          </tr>');
d.writeln ('          <tr>');
d.writeln ('            <td width="275" align="center">');
d.writeln ('            <div align="center">');
d.writeln ('              <center>');
d.writeln ('              <table border="1" cellpadding="3" cellspacing="0" style="border-collapse: collapse" bordercolor="#396BB5" width="275">');
d.writeln ('                <tr>');
d.writeln ('                  <td width="256">');
d.writeln ('                  <font face="Arial" size="2" color="#396BB5"><b>Error: </b>');
d.writeln (printmessage + '</font></td>');
d.writeln ('                </tr>');
d.writeln ('             </table>');
d.writeln ('             </center>');
d.writeln ('           </div>');
d.writeln ('           </td>');
d.writeln ('         </tr>');
d.writeln ('          <tr>');
d.writeln ('            <td height="21" width="256">&nbsp;</td>');
d.writeln ('          </tr>');
d.writeln ('          <tr>');
d.writeln ('            <td width="256">');
d.writeln ('            <p align="center"><a href="javascript:close()"><img border="0" src="/drm/blueclose.gif" width="82" height="22"></a></td>');
d.writeln ('          </tr>');
d.writeln ('          <tr>');
d.writeln ('            <td height="20" width="256">&nbsp;</td>');
d.writeln ('          </tr>');
d.writeln ('        </table>');
d.writeln ('        </center>');
d.writeln ('      </div>');
d.writeln ('        </td>');
d.writeln ('      </tr>');
d.writeln ('      </form>');
d.writeln ('    </table>');
d.writeln ('  </div>');
d.writeln ('  <p align="center">&nbsp;</p>');
d.writeln ('</body>');
d.writeln ('</html>');
	d.close;
	return;

}
