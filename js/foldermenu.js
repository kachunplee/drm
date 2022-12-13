var DRMFolderWin = null;
function DspStatus (msg) 
{
	window.status = msg;
	return true;
}

var openedMenu = null;
var nameItem;

function InitMenu ()
{
	if (bDHML) {
		document.onmouseover = mouseover;
		if (document.layers)
		{
			document.onmousedown = mouseover;
			document.captureEvents(Event.MOUSEOVER|Event.MOUSEOUT);
		}
	}
}

var FolderImg;

function menuX (x, width)
{
	return (x + width);
}

function menuY (y, height)
{
	return y;
}

function BlankPreview ()
{
	if (parent.DRMLstPrevHeader != null)
		parent.DRMLstPrevHeader.location = '/drm-bin/frmlstpreview?-h';
	if (parent.DRMLstPrevBody != null)
	{
		parent.DRMLstPrevBody.document.writeln('<html></html>');
		parent.DRMLstPrevBody.document.close();
	}
}

function OutFolder (folder, buildin)
{
	document.writeln('&nbsp;<img id=iMenu', buildin, 'Folder_', folder,
		' name=iMenu', buildin, 'Folder_', folder,
		' src=/drm/folderclosed.gif border=0 width=16 height=14>',
		'&nbsp;&nbsp;',
		'<a class=cFolder href="/drm-bin/frmlstctrl?',
		folder, '" target=DRMLstControl alt="Check ',
		folder, ' Folder"',
		' title="', frame.fd_read[folder], ' read ',
		frame.fd_unread[folder], ' unread">', folder, '</a><br>');
}


function SetFolderImg (fd)
{
	var img = document.images['iMenuFolder_' + fd];
	if (!img) img = document.images['iMenuBFolder_' + fd];

	if (img)
	{
		if (FolderImg)
		{
			if (img == FolderImg)
				return;

			FolderImg.src = '/drm/folderclosed.gif';
		}

		FolderImg = img;
		FolderImg.src = '/drm/folderopen.gif';
		BlankPreview();
	}
}

function OpenFolder ()
{
	CloseOpenedMenu();

	parent.DRMLstControl.document.location
		= '/drm-bin/frmlstctrl?' + nameItem;
	parent.DRMLstPrevHeader.location = '/drm-bin/frmlstpreview?-h';
}

function TransferSelected ()
{
	CloseOpenedMenu();

	// if (nameItem == CurrentFolder) window.open('/drm-bin/badfolder');

	var lstdsp = parent.DRMLstDisplay.document;
	lstdsp.maillist.actiontype.value = 'x' + nameItem;
	lstdsp.maillist.submit();
	lstdsp.close();
}

function TransferPreview ()
{
	CloseOpenedMenu();

	var hdrdsp = parent.DRMLstPrevBody.document;
	hdrdsp.maillist.actiontype.value = 'x' + nameItem;
	hdrdsp.maillist.submit();
	hdrdsp.close();
	BlankPreview();
	parent.DRMLstDisplay.location.reload();
}

function DoEmptyTrash ()
{
	var lstdsp = parent.DRMLstDisplay.document;
	lstdsp.maillist.actiontype.value = 'emptytrash';
	lstdsp.maillist.submit();
	lstdsp.close();
}

function DoEmptyInBox ()
{
	var lstdsp = parent.DRMLstDisplay.document;
	lstdsp.maillist.actiontype.value = 'emptyinbox';
	lstdsp.maillist.submit();
	lstdsp.close();
}

function NewFolder ()
{
	if(foldercount < maxcount)
	{
		var winurl = '/drm-bin/addfolder?-A';
		if ((DRMFolderWin != null) && !DRMFolderWin.closed)
			DRMFolderWin.location.href = winurl;
		else
		{
			var winname = "DRMFolderWin";
			DRMFolderWin = window.open(winurl, winname,
			"width=300,height=200,resizable=no,scrollbars=no,toolbar=no,status=yes");
		}

		DRMFolderWin.focus();
	}
	else
	{
		OpenErrorWin('maxbox')
		return;
	}
}

function RenameFolder ()
{
	CloseOpenedMenu();

	var winurl = '/drm-bin/renamefolder?' + nameItem;
	if ((DRMFolderWin != null) && !DRMFolderWin.closed)
		DRMFolderWin.location.href = winurl;
	else
	{
		var winname = "DRMFolderWin";
		DRMFolderWin = window.open(winurl, winname,
			"width=300,height=200,resizable=no,scrollbars=no,toolbar=no,status=yes");
	}

	DRMFolderWin.focus();
}

function DeleteFolder ()
{
	CloseOpenedMenu();

	var winurl = '/drm-bin/delfolder?' + nameItem;
	if ((DRMFolderWin != null) && !DRMFolderWin.closed)
		DRMFolderWin.location.href = winurl;
	else
	{
		var winname = "DRMFolderWin";
		DRMFolderWin = window.open(winurl, winname,
			"width=300,height=200,resizable=no,scrollbars=no,toolbar=no,status=yes");
	}

	DRMFolderWin.focus();
}

function EmptyTrash ()
{
	var winurl = '/drm-bin/emptytrash';
	if ((DRMFolderWin != null) && !DRMFolderWin.closed)
		DRMFolderWin.location.href = winurl;
	else
	{
		var winname = "DRMFolderWin";
		DRMFolderWin = window.open(winurl, winname,
			"width=300,height=200,resizable=no,scrollbars=no,toolbar=no,status=yes");
	}

	DRMFolderWin.focus();
}

function EmptyInBox ()
{
	var winurl = '/drm-bin/emptyinbox';
	if ((DRMFolderWin != null) && !DRMFolderWin.closed)
		DRMFolderWin.location.href = winurl;
	else
	{
		var winname = "DRMFolderWin";
		DRMFolderWin = window.open(winurl, winname,
			"width=300,height=200,resizable=no,scrollbars=no,toolbar=no,status=yes");
	}

	DRMFolderWin.focus();
}

function OutStorageBar ()
{
	document.writeln('<p>');
	document.writeln('<div class=cStorageBar>');
	document.writeln('<font face="Arial" size="2" color="#396BB5">',
		'Storage Left: ', frame.storage_left, ' MB</font><br>');
	document.writeln('<table width=135 border=1 cellpedding=0 ',
		'cellspacing=0 bordercolor=#000000>');
	document.writeln('<tr height=7>');
	if (frame.storage_use > 0)
		document.writeln('<td bgcolor=#21599C width=',
			frame.storage_use,
			'><img src=/drm/spacer_1pixel.gif height=1 width=1>',
			'</td>');
	if (frame.storage_remain > 0)
		document.writeln('<td bgcolor=#EAEFF8 width=',
			frame.storage_remain,
			'><img src=/drm/spacer_1pixel.gif height=1 width=1>',
			'</td>');
	document.writeln('</tr>');
	document.writeln('</table>');
	document.writeln('</div>');
}
