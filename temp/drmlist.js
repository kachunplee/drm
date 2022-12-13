//
//
//
function DRMListRow(id, doRow)
{
	var classRoot = 'drm_row_';
	this.id = id;
	this.row_id = doRow.id;
	this.parity = doRow.className.substr(classRoot.length);
	this.msg = doRow.getAttribute('msg');
}

DRMListRow.prototype.fMouseDown = function()
{
	var row_obj = this;
	return function(e)
	{
		drmDrag.listMouseDown(e, row_obj)
	}
}

DRMListRow.prototype.fMouseUp = function()
{
	var row_obj = this;
	return function()
	{
		drmDrag.listMouseUp(row_obj);
	}
}

DRMListRow.prototype.fMouseOut = function(row_id)
{
	var row_obj = this;
	return function()
	{
		var obj = Utilities.getElement(row_id);
		obj.style.color = drmDrag.isSelect(row_obj) ? '#eeeeee' :
                    ((obj.style.fontWeight == 'bold') ? '#000000' : '#666666');
		drmDrag.listMouseOut(row_obj);
	}
}

DRMListRow.prototype.fMouseOver = function(row_id)
{
	var row_obj = this;
        return function()
        {
                Utilities.getElement(row_id).style.color = '#fd1813';
        }
}

DRMListRow.prototype.setdoMethod = function(doRow)
{
	  doRow.onmouseup = this.fMouseUp(),
	  doRow.onmousedown = this.fMouseDown()
	  doRow.onmouseout = this.fMouseOut(this.row_id),
	  doRow.onmouseover = this.fMouseOver(this.row_id)
}

function listPreview (doRow)
{
	return openPreview(doRow.getAttribute('msg'), doRow.id);
}

function listArticle (doRow)
{
	return openArticle(doRow.getAttribute('msg'), doRow.id);
}

//
//
function listReady (datagrid)
{
	if (!datagrid)
		datagrid = document.getElementById("drm_list_dsp");
	datagrid.style.cursor = 'pointer';
	document.body.style.cursor = 'default';
}

function listWait (datagrid)
{
        document.body.style.cursor = 'wait';
	if (!datagrid)
		datagrid = document.getElementById("drm_list_dsp");
        datagrid.style.cursor = 'wait';
}

//
//
//
DRMList = {};

DRMList.init = function()
{
	listRow = new Array();
}

DRMList.dspRow = function (id, items)
{
	var row_id = 'drm_row_' + id;

	var msg = items[0];
	var status = items[1] - 0;
	var size = items[2];
	var att = (items[3] == 'yes');

	var parity = id%2 ? 'odd' : 'even';

	var icon;
	switch(status)
	{
	case 0:
		icon = 'mailclosed.gif';
		break;
	case 1:
		icon = 'mailsend.gif';
		break;
	case 2:
		icon = 'mailreply.gif';
		break;
	case 3:
		icon = 'mailforward.gif';
		break;
	case 4:
		icon = 'mailopened.gif';
		break;
	}

	var rowstg = '<div id="' + row_id + '"'
  			+ ' class="drm_row_' + parity + '"'
			+ ' msg="' + msg + '"'
			+ ' onclick="listPreview(this)"'
			+ ' ondblclick="listArticle(this)"';

	if (status == 0)
		rowstg += ' style="font-weight: bold; color: #000000"';

	rowstg += '><div id="drm_col_' + id + '_s"'
			+ ' class="drm_col_s">'
			+ '<img src="/drm/img/' + icon
			+ '" width="24" title="' 
			+ size + '" alt="' + size + '">';

	if (att)
		rowstg += '<img src="/drm/img/mailattach.gif" width="12">';
	rowstg += '</div>';

	for(var col=0; col<3; col++)
	{
		rowstg += '<div id="drm_col_' + id + '_' + col + '"'
			+ ' class="drm_col_' + col + '">'
			+ items[col+4] + '</div>';
	}

	rowstg += '</div>';
	return rowstg;
}

DRMList.display = function ()
{
	if(Ajax.checkReadyState('loading') == "OK")
	{
		var datagrid = document.getElementById("drm_list_dsp");
		
		var liststg = ''
		var resp =  Ajax.getResponse();
		var iEnd;
		var iLine;
		var id = 0;
		var unread = 0;
		var nummail = 0;
		var inbox = (currentFolder == 'in');
		for (iLine = 0; ; iLine = iEnd+2)
		{
			iEnd = resp.indexOf("<\n", iLine);
			if (iEnd < 0) break;
			var items = resp.substr(iLine, iEnd-iLine).split('<');
			nummail++;
			if ((items[1] - 0) == 0) unread++;
			if (inbox);
				liststg += DRMList.dspRow(id++, items);
		}

		// Update folder nummail, unread
		// get obj 

		if (!inbox)
		{
			AjaxUpdater.saving = false;
			startAutoCheck();
			//listReady(datagrid); //?
			//endWait();
			return;
		}

		if (liststg)
			datagrid.innerHTML = liststg;
		else
			datagrid.innerHTML = '<div id="drm_list_outer"><div id="drm_list_msg">Folder is empty</div></div>';

		AjaxUpdater.saving = false;

		var n = datagrid.scrollHeight - datagrid.offsetHeight - 1;
		if (n > 0) datagrid.scrollTop = n;

		var classRoot = 'drm_row_';
		var n = 0;
		for (var doRow = datagrid.firstChild; doRow; doRow = doRow.nextSibling)
		{
			if ((doRow.nodeType == 1) &&
			     (doRow.className.substr(0, classRoot.length) == classRoot))
			{
				var objRow = new DRMListRow(n++, doRow);
				objRow.setdoMethod(doRow);
				listRow.push(objRow);
				
			}
		}

		startAutoCheck();

		listReady(datagrid);
		endWait();
	}
}

DRMList.updinfolderstat = function ()
{
	if(Ajax.checkReadyState('loading') == "OK")
	{
		var liststg = ''
		var resp =  Ajax.getResponse();
		var iEnd;
		var iLine;
		var id = 0;
		for (iLine = 0; ; iLine = iEnd+2)
		{
			iEnd = resp.indexOf("<\n", iLine);
			if (iEnd < 0) break;
			var items = resp.substr(iLine, iEnd-iLine).split('<');
			liststg += DRMList.dspRow(id++, items);
		}

		var datagrid = document.getElementById("drm_list_dsp");
		if (liststg)
			datagrid.innerHTML = liststg;
		else
			datagrid.innerHTML = '<div id="drm_list_outer"><div id="drm_list_msg">Folder is empty</div></div>';

		AjaxUpdater.saving = false;

		var n = datagrid.scrollHeight - datagrid.offsetHeight - 1;
		if (n > 0) datagrid.scrollTop = n;

		var classRoot = 'drm_row_';
		var n = 0;
		for (var doRow = datagrid.firstChild; doRow; doRow = doRow.nextSibling)
		{
			if ((doRow.nodeType == 1) &&
			     (doRow.className.substr(0, classRoot.length) == classRoot))
			{
				var objRow = new DRMListRow(n++, doRow);
				objRow.setdoMethod(doRow);
				listRow.push(objRow);
				
			}
		}

		startAutoCheck();

		listReady(datagrid);
		endWait();
	}
}

var refreshcnt = 0;

function displayList (folder, sort)
{
	showWait();

	var datagrid = document.getElementById("drm_list_dsp");
	listWait(datagrid);

	stopAutoCheck();

	// empty screen
	if (datagrid.innerHTML)
		datagrid.innerHTML = '';

	// new drag array
	drmDrag = new DRMDrag();

	cSort = sort;
	datagrid.style.fontSize = (fontsize == 'xx-small') ? '70%'
		: ((fontsize == 'x-small') ? '80%' : '90%');

	var eTitle = document.getElementById("drm_list_title");
	eTitle.innerHTML = folder.substr(0, 1).toUpperCase() + folder.substr(1) 				+ ' Box';

	sort = (sort && (sort.length > 0)) ? sort = '+-s' + sort : '';
	DRMList.init();
	AjaxUpdater.Update("GET", binDir + "folder?" + folder + '+-X' +
			sort + ++refreshcnt + Math.random(),
			DRMList.display);
}

function updateInFolder ()
{
	stopAutoCheck();

	AjaxUpdater.Update("GET", binDir + "folder?" + folder + '+-X' +
			sort + ++refreshcnt + Math.random(),
			DRMList.display);
}

var checkID = -1;
var cSort;

function time2dspFolder ()
{
	// BlankPreview();
	displayList('in', cSort);
}

function time2updInFolder ()
{
	updateInFolder();
}

function stopAutoCheck ()
{
	if (checkID >= 0)
	{
		clearTimeout(checkID);
		checkID = -1;
	}
}

function startAutoCheck ()
{
	stopAutoCheck();

	if (checktime == 0)
		return;

	if (currentFolder != 'in')
		checkID = setTimeout(time2updInFolder, checktime*1000*60);
	else
		checkID = setTimeout(time2dspFolder, checktime*1000*60);
}

function MarkMsgOpen(row_id)
{
	if (row_id == null)
		return;

	var e = document.getElementById(row_id);
	var img = e.getElementsByTagName('img');
	var imgclosed = 'mailclosed.gif';
	var imgnow = img[0].src;
	if (img && (imgnow.substr(imgnow.length-imgclosed.length) == imgclosed))
	{
		img[0].src = '/drm/img/mailopened.gif';
		e.style.fontWeight = 'normal';
	}
}
