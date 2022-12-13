// revised: 2008-01-06

var DRMFolderWin = null;

var openedMenu = null;
var nameItem;

var FolderImg;

//
//
//
function menuX (x, width)
{
	return x + (bIE ? 0 : width/2);
}

function menuY (y, height)
{
	return (y + height/2);
}

//
//
function BlankPreview ()
{
	if (!previewframe)
		return;
	setPrevwTitle('', '', '', '', null) 
	drmPreview.location.href = '/drm5/drmprevw.asp';
}

function folderMouseOver (target, folder)
{
	if (!drmDrag.isDrag())
		return;

	if (currentFolder != folder)
	{
		target.style.cursor = 'crosshair';

		var img = target.getElementsByTagName('img');
		if (img && img[0])
		     img[0].src = '/drm/img/transfericon.gif';
	}
	else
	{
		target.style.cursor = 'no-drop';
	}
		
}

function folderDefault (target, folder)
{
	target.style.cursor = 'pointer';
	var imgs = target.getElementsByTagName('img');
	var img = (folder == currentFolder)
		? '/drm/img/folderopen.gif'
		: '/drm/img/folderclosed.gif';
	var imgnow = imgs[0].src;
	if (imgnow.substr(imgnow.length-img.length) != img)
		imgs[0].src = img;
}

function folderMouseOut (target, folder)
{
	folderDefault(target, folder);
}

function folderMouseUp (target, folder)
{
	folderDefault(target, folder);
	drmDrag.isDrop(target, folder);
}

function imgMouseOver (target, folder)
{
	if (drmDrag.isDrag())
	{
		folderMouseOver(target, folder);
		return false;
	}

	var obj = document.images['iMenuFolder_' + folder];
	if (!obj) obj = document.images['iMenuBFolder_' + folder];

	if (obj)
		showMenu(obj);
	
	return false;
}


var dirClosed = '';

function outFolder (folder, buildin, state)
{
	var fs = new Array();
	var pfolder;

	var fhide = (dirClosed == '') ? '' : 'display:none;';

	if (folder.charAt(folder.length-1) == '\/')
	{
	    var iconimg;
	    var iconcmd;

	    // Folders directory -> default to open
	    if (state != '-')
	    {
		// This folder is opened
		if (dirClosed != '')
		{
			// But previous folder is closed
			if (folder.substr(0, dirClosed.length)
				!= dirClosed) 
			{
				// Not a subtree of it though
				// Change to display
				fhide = '';
				dirClosed = '';
			}
		}

		iconimg = 'diropen.gif';
		iconcmd = 'CloseFolderDir';
	    }
	    else
	    {
		// This folder is closed
		if (dirClosed == ''
		   || (folder.substr(0, dirClosed.length) != dirClosed)) 
		{
			// Not a subtree of previous closed folder
			// Display this dir but mark closed
			fhide = '';
			dirClosed = folder;
		}

		iconimg = 'dirclose.gif';
		iconcmd = 'OpenFolderDir';
            }

	    fs = folder.split('\/');
	    if (fs[(fs.length-1)] == '')
		fs.pop();

	    var foldername;
	    var ispaces = '';
	    if (fs.length == 1)
	    {
		foldername = fs[0];
	    }
	    else
	    {
		var i;
		for(i = 1; i < fs.length; i++)
			ispaces += "&nbsp;&nbsp";
		foldername = fs[(fs.length-1)];
	    }

	    document.writeln(
		'<div id="div_', folder, '" style="height:18px;',
		'margin-bottom:3px;', fhide,
		';white-space:nowrap;">', ispaces,
		'<a id=aFolder_', folder, ' href="javascript:',
		iconcmd, '(\'', folder,
		'\')"><img id=iconFolder_', folder,
		' align=top border=0 width=9 ',
		'height=9 src=/drm/img/', iconimg, '></a><span class=cFolder',
		' onmouseup="folderDMouseUp(this,\'', folder, '\')"',
		' onmouseover="folderDMouseOver(this, \'', folder, '\')"',
		' onmouseout="folderDMouseOut(this, \'', folder, '\')">',
		'<img align=top id=iMenuFolderDir_', folder,
		' onmouseover="imgDMouseOver(this, \'', folder, '\')"',
		' name=iMenuFolderDir_', folder,
		' src=/drm/img/folderdiropen.gif border=0 width=21 height=17>',
		'&nbsp;',
		' <a id="fstat_', folder, '" alt="Check ', folder, ' Folder">',
		'&nbsp;',
		foldername.substr(0,1).toUpperCase(), foldername.substr(1),
		'</a></span><br></div>'
	    );

	    return;
	}

	// regular folder
	fs = folder.split("/");
	var ispaces = '';

	for (var i = 1; i < fs.length; i++)
	{
		ispaces += '&nbsp;&nbsp;';
	}

	if ((dirClosed != '')
	     && (folder.substr(0, dirClosed.length) != dirClosed)) 
	{
		// Not a subtree of previous closed folder
		// Show it
		fhide = '';
		dirClosed = '';
	}

	var foldername = fs[(fs.length-1)];
	var imgsrc = 'folderclosed.gif';
	document.writeln(
		'<div id="div_', folder, '" style="height:18px;',
		'margin-bottom:3px;', fhide,
		';white-space:nowrap;">', ispaces,
		'<img align=top border=0 width=9 height=9 ',
		'src=/drm/img/spacer_1pixel.gif><span class=cFolder',
		' onmouseup="folderMouseUp(this,\'', folder, '\')"',
		' onmouseover="folderMouseOver(this, \'', folder, '\')"',
		' onmouseout="folderMouseOut(this, \'', folder, '\')"',
		'style="height:18px;">',
		'<img align=top id=iMenu', buildin, 'Folder_', folder,
		' onmouseover="imgDMouseOver(this, \'', folder, '\')"',
		' name=iMenu', buildin, 'Folder_', folder,
		' src=/drm/img/', imgsrc, ' border=0 width=21 height=17>',
		'&nbsp;',
		' <a id="fstat_', folder, '" alt="Check ', folder, ' Folder"',
		' title="', frame.fd_read[folder], ' read ',
		frame.fd_unread[folder], ' unread"',
		' href="javascript:OpenFolder(\'', folder, '\')">&nbsp;',
		foldername.substr(0,1).toUpperCase(), foldername.substr(1),
		'</a></span><br></div>'
	);
}


function OutStorageBar ()
{
	document.writeln('<p>');
	document.writeln('<div class=cStorageBar>');
	document.writeln('Storage Left: ', frame.storage_left, ' MB<br>');
	document.writeln('<table width=135 border=1 cellpedding=0 ',
		'cellspacing=0 bordercolor=#000000>');
	document.writeln('<tr height=7>');
	if (frame.storage_use > 0)
		document.writeln('<td bgcolor=#21599C width=',
			frame.storage_use,
			'><img src=/drm/img/spacer_1pixel.gif height=1 width=1>',
			'</td>');
	if (frame.storage_remain > 0)
		document.writeln('<td bgcolor=#EAEFF8 width=',
			frame.storage_remain,
			'><img src=/drm/img/spacer_1pixel.gif height=1 width=1>',
			'</td>');
	document.writeln('</tr>');
	document.writeln('</table>');
	document.writeln('</div>');
}

//
//
//
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

			FolderImg.src = '/drm/img/folderclosed.gif';
		}

		FolderImg = img;
		FolderImg.src = '/drm/img/folderopen.gif';
	}
}

var currentSort = '';

function dspFolder (folder, sort)
{
	// revert
	if (sort && (sort == currentSort))
	{
		sort = String.fromCharCode(sort.charCodeAt(0)+1);
	}

	currentSort = sort;
	currentFolder = folder;
	BlankPreview();
	SetFolderImg (folder)
	displayList(folder, sort, false);

	if (currentFolder == 'out')
	{
		document.getElementById('who_col_span').innerHTML = 'To';
		var obj = document.getElementById('who_prevw_span');
		if (obj)
			obj.innerHTML = 'To';
	}
	else
	{
		document.getElementById('who_col_span').innerHTML = 'From';
		var obj = document.getElementById('who_prevw_span');
		if (obj)
			obj.innerHTML = 'From';
	}

	var html_up = '<img align=top src="/drm/img/blue_arrowup.gif" border="0" width="10" height="15">';
	var html_down = '<img align=top src="/drm/img/blue_arrowdown.gif" border="0" width="10" height="15">';
	var sortspan;
	if (sort == "s" || sort == "t")
	{
		sortspan = document.getElementById('subject_sort_span');
		sortspan.innerHTML = ((sort == "s") ? html_up : html_down);
		sortspan = document.getElementById('date_sort_span');
		sortspan.innerHTML = "";
		sortspan = document.getElementById('from_sort_span');
		sortspan.innerHTML = "";
	}
	else if(sort == "w" || sort == "x")
	{
		sortspan = document.getElementById('from_sort_span');
		sortspan.innerHTML = ((sort == "w") ? html_up : html_down);
		sortspan = document.getElementById('date_sort_span');
		sortspan.innerHTML = "";
		sortspan = document.getElementById('subject_sort_span');
		sortspan.innerHTML = "";
	}
	else if(sort == "d" || sort == "e")
	{
		sortspan = document.getElementById('date_sort_span');
		sortspan.innerHTML = ((sort == "d") ? html_up : html_down);
		sortspan = document.getElementById('subject_sort_span');
		sortspan.innerHTML = "";
		sortspan = document.getElementById('from_sort_span');
		sortspan.innerHTML = "";
	}
	else
	{
		sortspan = document.getElementById('date_sort_span');
		sortspan.innerHTML = html_up;
		sortspan = document.getElementById('subject_sort_span');
		sortspan.innerHTML = "";
		sortspan = document.getElementById('from_sort_span');
		sortspan.innerHTML = "";
	}
}

function sortFolder (sort)
{
	dspFolder(currentFolder, sort);

}


function OpenFolder (folder)
{
	if (openedMenu)
		CloseOpenedMenu();

	var fname = nameItem;
	if (folder && folder != '')
	{
		fname = folder;
	}

	dspFolder(fname);
}

//
//
//
function updateFolderStat (folder, read, unread)
{
	if (!read)
	{
		read = 0;
		unread = 0;
	}

	//fd_read[folder] = read;
	//fd_unread[folder] = unread;
	var folder = document.getElementById('fstat_' + folder);
	if (folder)
	{
		folder.title = read + ' read ' +
				unread + ' unread';
	}
	
}

function refreshFolder (folder, read, unread)
{
	updateFolderStat(folder, read, unread);
	if (currentFolder != folder)
		return;

	// Temp replace the whole browser
	window.location.replace('/drm5/index.asp?folder=' + folder);
	// Ajax error
	// BlankPreview();
	// SetFolderImg (folder)
	// displayList(folder, currentSort, false);
}

//
//
//
function showWait ()
{
	var dsp = document.getElementById("drm_display");
	var ele = document.getElementById("drm_list_mask");
	ele.style.width = dsp.offsetWidth + 'px';
	ele.style.height = dsp.offsetHeight + 'px';
	ele.style.display = 'block';
}

function endWait ()
{
	var ele = document.getElementById("drm_list_mask");
	ele.style.display = 'none';
}

//
//
//
var transfering = false;
function TransferDone ()
{
	if(Ajax.checkReadyState('loading') == "OK")
	{
		listReady();
		//xdebug(Ajax.getResponse());
		dspFolder(currentFolder);
		transfering = false;
	}
}

function TransferStart (folder)
{
	if (transfering)
		return;

	showWait();
	transfering = true;

	listWait();

	var postdata = 'actiontype=x' + encodeURI(folder);
	postdata += '&mboxname=' + encodeURI(currentFolder);

	var sel = '';
	for (var n = 0; n < selects.length; n++)
	{
		sel += selects[n].msg + "\n";
		postdata += '&' + selects[n].msg.replace(/\+/g, ",") + '=yes';
	}

	//xdebug('<p>MouseDrop ' + folder + '<br>' + sel + '<br>');
		
	try
	{
		AjaxUpdater.Update("POST", '/drm-bin/mailxfer?-X',
			TransferDone, postdata);
	}
	catch(e)
	{
		//xdebug ("List Transfer Ajax error " + e);
	}
}

function TransferSelected ()
{
	if (openedMenu)
		CloseOpenedMenu();

	TransferStart(nameItem);
}

//
//
//
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

function AddFolder ()
{
	if (openedMenu)
		CloseOpenedMenu();

	if(foldercount < maxcount)
	{
		var folderpath = '';
		if (nameItem && nameItem != '')
			folderpath = nameItem;
		var winurl = '/drm-bin/addfolder?-A+' + folderpath;
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
	if (openedMenu)
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
	var winurl = '/drm-bin/emptytrash?+' + Math.random();
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
	var winurl = '/drm-bin/emptyinbox?+' + Math.random();
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

var marking = false;
function MarkDone ()
{
	if(Ajax.checkReadyState('loading') == "OK")
	{
		listReady();
		//xdebug(Ajax.getResponse());
		dspFolder(currentFolder);
		marking = false;
	}
}

function MarkStart (marktype)
{
	if (marking)
		return;

	marking = true;

	listWait();

	var postdata = 'actiontype=' + marktype;
	postdata += '&mboxname=' + encodeURI(currentFolder);

	var sel = '';
	for (var n = 0; n < selects.length; n++)
	{
		sel += selects[n].msg + "\n";
		postdata += '&' + selects[n].msg.replace(/\+/g, ",") + '=yes';
	}

	try
	{
		AjaxUpdater.Update("POST", '/drm-bin/mailmark?-X',
			MarkDone, postdata);
	}
	catch(e)
	{
		//xdebug ("List Transfer Ajax error " + e);
	}
}

function MarkReadSelected ()
{
	closeMenu(0);
	MarkStart('r');
}

function MarkUnreadSelected ()
{
	closeMenu(0);
	MarkStart('u');
}

//
// Actually, the folder panel is not in a frame anymore...
// However, parent.parent just point back to itself.
//
var frame = parent.parent;

function updateFolderDir (dirID, bOpen)
{
	for(i = 0; i < frame.Folders.length; i++)
	{
	    var foldername = frame.Folders[i];
	    if (foldername == dirID)
	    {
		var iconImg;
		var iconCmd;

	    	if (bOpen)
	    	{
			frame.FDiropens[i] = '+';
			iconImg = 'diropen.gif'
			iconCmd = 'CloseFolderDir'
		}
		else
		{
			frame.FDiropens[i] = '-';
			iconImg = 'dirclose.gif'
			iconCmd = 'OpenFolderDir'
	    	}

		obj = document.getElementById("iconFolder_" + dirID);
		if (obj) obj.src = '/drm/img/' + iconImg;
	    	obj = document.getElementById("aFolder_" + dirID);
	    	if (obj)
			obj.href = 'javascript:' + iconCmd + '(\'' + dirID + '\')';
		break;
	    }
	}
}

function refreshFolderTree (response)
{
	// if (response.match(/^OK:/))

	var fhide = '';
	var dClosed = '';
	for(i = 0; i < frame.Folders.length; i++)
	{
	    var folder = frame.Folders[i];

	    var dID = "div_" + folder;
	    var obj = document.getElementById(dID);
	    if (!obj) continue;		// ?????

	    fhide = (dClosed == '') ? '' : 'none';

	    if (folder.charAt(folder.length-1) == '\/')
	    {
		// Folders directory -> default to open
		if (FDiropens[i] != '-')
		{
			// This folder is opened
			if (dClosed != '')
			{
				// But previous folder is closed
				if (folder.substr(0, dClosed.length)
					!= dClosed) 
				{
					// Not a subtree of it though
					// Display it
					fhide = '';
					dClosed = '';
				}
			}
		}
		else
		{
			// This folder is closed
			if (dClosed == ''
			   || (folder.substr(0, dClosed.length) != dClosed)) 
			{
				// Not a subtree of previous closed folder
				// Display this dir but mark closed
				fhide = '';
				dClosed = folder;
			}
		}

	    }
	    else
	    {
	        if ((dClosed != '')
		     && (folder.substr(0, dClosed.length) != dClosed)) 
	        {
			// Not a subtree of previous closed folder
			// Show it
			fhide = '';
			dClosed = '';
	        }
	    }
	    var div_obj = document.getElementById('div_' + folder);
	    div_obj.style.display = fhide;
	}
}

function OpenFolderDir (folder)
{
	if(!folder || folder == '')
		folder = nameItem;

	updateFolderDir (folder, true);
	ajaxRequest("/drm-bin/folderopen?" + folder + '+' + Math.random(),
		function (response) { refreshFolderTree(response); });
}

function CloseFolderDir (folder)
{
	if(!folder || folder == '')
		folder = nameItem;

	updateFolderDir (folder, false);
	ajaxRequest("/drm-bin/folderclose?" + folder + '+' + Math.random(),
		function (response) { refreshFolderTree(response); });
}

function AddFolderDir ()
{
	if (openedMenu)
		CloseOpenedMenu();

	var folderpath = '';
	if (nameItem && nameItem != '')
		folderpath = nameItem;
	var winurl = '/drm-bin/addfolderdir?-A+' + folderpath;
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

function DeleteFolderDir ()
{
	CloseOpenedMenu();

	var dID = "div_" + nameItem;
	var obj = document.getElementById(dID);
	if (!obj)
		return;

	var fs = nameItem.split('\/');
	var dLen = fs.length;
	for (var div_obj = obj.nextSibling; div_obj;
		div_obj = div_obj.nextSibling)
	{
		if (div_obj.nodeName == 'DIV')
		{
			// 
			if (obj.id != div_obj.id.substring(0, obj.id.length))
				break;

			OpenErrorWin('folderdir')
			return;
		}
	}

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

function folderDMouseOver (target, folder)
{
	if (!drmDrag.isDrag())
		return;

//	if (currentFolder != folder)
//	{
//		target.style.cursor = 'crosshair';
//
//		var img = target.getElementsByTagName('img');
//		if (img && img[0])
//		     img[0].src = '/drm/img/transfericon.gif';
//	}
//	else
//	{
		target.style.cursor = 'no-drop';
//	}
		
}

function folderDDefault (target, folder)
{
	target.style.cursor = 'pointer';
/*
	var imgs = target.getElementsByTagName('img');
	var img = (folder == currentFolder)
		? '/drm/img/emptyinbox.gif'
		: '/drm/img/addfolder.gif';
	var imgnow = imgs[0].src;
	if (imgnow.substr(imgnow.length-img.length) != img)
		imgs[0].src = img;
*/
}

function folderDMouseOut (target, folder)
{
	folderDDefault(target, folder);
}

function folderDMouseUp (target, folder)
{
	folderDDefault(target, folder);
	drmDrag.isDrop(target, folder);
}

function imgDMouseOver (target, folder)
{
	if (drmDrag.isDrag())
	{
		folderDMouseOver(target, folder);
		return false;
	}

	var obj = document.images['iMenuFolderDir_' + folder];

	if (obj)
		showMenu(obj);
	
	return false;
}
