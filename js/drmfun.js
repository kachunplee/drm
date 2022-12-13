var bIE = (navigator.appName.indexOf("Internet Explorer") != -1);
var bIEMac = (bIE && (navigator.userAgent.indexOf("Mac") != -1));
var bDHML = (parseInt( navigator.appVersion ) >= 4);

//
//
//
_debug = null;

function xdebug (msg)
{
	if ((_debug == null) || (_debug.closed)) {
		_debug = window.open("", "EventMonitor",
			"width=300,height=600,scrollbars,resizable,menubar");
		_debug.document.open("text/html");
	}
	_debug.document.writeln(msg);
}

//
//
//
function disableSelection(target)
{
	if (typeof target.onselectstart!="undefined") //IE route
		target.onselectstart=function() { return false; }
	else if (typeof target.style.MozUserSelect!="undefined") //Firefox route
		target.style.MozUserSelect="none"
	else //All other route (ie: Opera)
		target.onmousedown=function(){ return false; }
	target.style.cursor = "default";
}

//
//
//
function setPrevwTitle (subj, from, email, folder, msg) 
{
	var osubj = document.getElementById("drm_prevw_subj");
	var ofrom = document.getElementById("drm_prevw_from");
	var oprn = document.getElementById("drm_prevw_print");

	if (msg == null)
	{
		if (osubj && osubj.innerHTML) osubj.innerHTML = '';
		if (ofrom && ofrom.innerHTML) ofrom.innerHTML = '';
		if (oprn && oprn.innerHTML) oprn.innerHTML = '';
		return;
	}

	osubj.innerHTML = '<a href="javascript:OpenMsgWin(\''
		+ binDir + 'frmmessage?'
		+ folder + '+' + msg + '\')" title="Open Message">'
		+ subj + '</a>';
	ofrom.innerHTML = (email.length > 0)
		? '<a href="mailto:' + email + '">' + from + '</a>'
		: from;
	oprn.innerHTML = '<a href="javascript:OpenPrnMsgWin(\''
		+ binDir + 'frmprnmsg?'
		+ folder + '+-M+' + msg + '\')" title="Open Printable Message"><img src=/drm/img/prnver.gif border=0 width=112 height=16></a>';
}

//
//
//
function openArticle (msg, row_id)
{
	MarkMsgOpen(row_id);
	OpenMsgWin(binDir + 'frmmessage?' + currentFolder + '+' + msg);
}

function openPreview (msg, row_id)
{
	if (previewframe && drmPreview != null)
	{
		MarkMsgOpen(row_id);
		drmPreview.location.href= binDir + 'message?' + currentFolder + '+-X+-P+' + msg;
	}
}

//
//
//
function UpdFolder (fd, rd, ur)
{
	currentFolder = fd;
	if (!bLoad)
	{
		fd_read[fd] = rd;
		fd_unread[fd] = ur;
		return;
	}

	if ((rd != fd_read[fd]) || (ur != fd_unread[fd]))
	{
		top.status = 'Folder ' + fd + '<>' + currentFolder + ' '
			+ rd + '/' + ur + '<>' + fd_read[fd] + '/' + fd_unread[fd];
		fd_read[fd] = rd;
		fd_unread[fd] = ur;
		DRMFolder.location.reload();
	}
	else
	{
		// change image
		DRMFolder.SetFolderImg(fd);
	}
}

function SetUserFolders (userfolder, newfolders)
{
	var i;
	Folders.length = userfolder;
	for (var i = 0; i < userfolder; i++)
	{
		Folders[i] = newfolders[i];
	}
}
