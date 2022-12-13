//
function DRMDrag ()
{
	selects = new Array();
	lastClick = null;
	mouseDown = false;
	ctrlDown = false;
	clearSelects = false;
	draging = false;
}

DRMDrag.prototype.fDocMouseUp = function ()
{
	var obj = this;
	return function ()
	{
		obj.mouseUp();
	}
}

DRMDrag.prototype.isSelect = function (row_obj)
{
	for(var n = 0; n < selects.length; n++)
	{
		if (selects[n] == row_obj)
			return true;
	}

	return false;
}


DRMDrag.prototype.select = function (row_obj)
{
	selects.push(row_obj);

	var row_id = row_obj.row_id;
	var row = Utilities.getElement(row_id);
	row.style.color = '#eeeeee';
	row.style.backgroundColor = '#3173CE';
}

DRMDrag.prototype.deSelect = function (row_obj)
{
	var row_id = row_obj.row_id;
	var row = Utilities.getElement(row_id);
	row.style.backgroundColor = (row_obj.parity == "even")
				? '#f3f3f3' : '#fff';
	row.style.color = (row.style.fontWeight == 'bold') ?
				'#000000' : '#666666';
}

DRMDrag.prototype.selectAll = function ()
{
	for(var n = 0; n < listRow.length; n++)
	{
		if (!this.isSelect(listRow[n]))
			this.select(listRow[n]);
	}
}

DRMDrag.prototype.listMouseDown = function (e, row_obj)
{
	e = e || window.event;

	ctrlDown = e.metaKey ? e.metaKey : e.ctrlKey;
	if (!ctrlDown)
	{
		if(e.shiftKey)
		{
			// search for 1st row
			var rowidx = -1;
			for (var n = 0; n < selects.length; n++)
			{
				if ((rowidx < 0) || (selects[n].id < rowidx)) 
					rowidx = selects[n].id;
				this.deSelect(selects[n]);
			}

			if (rowidx >= 0)
			{
				selects = new Array();
				// select from last select to row
				var inc = (rowidx < row_obj.id) ? 1 : -1;
				while (rowidx != row_obj.id)
				{
					this.select(listRow[rowidx]);
					rowidx += inc;
				}
		     	}
		}
		else {
			if (selects.length == 1)
			{
				this.deSelect(selects[0]);
				selects = new Array();
			}
			else
				clearSelects = true;
		}

		this.select(row_obj);
	}

	lastClick = row_obj;
	mouseDown = true;
}

DRMDrag.prototype.mouseUp = function ()
{
	if (ctrlDown)
	{
		ctrlDown = false;

		// toggle key
		var l = selects.length;
		for (var n = 0; n < l; n++)
		{
			if (selects[n] == lastClick)
			{
				selects.splice(n, 1);
				this.deSelect(lastClick);
				break;
			}
		}

		if (n >= l)
		{
			this.select(lastClick);
		}
	}
	else
	{
		if (clearSelects)
		{
			clearSelects = false;
			for (var n = 0; n < selects.length; n++)
			{
				if (selects[n] != lastClick)
					this.deSelect(selects[n]);
			}
			selects = new Array();
			selects.push(lastClick);
		}
	}

	mouseDown = false;
	draging = false;

	Utilities.getElement('drm_list_dsp').style.cursor = 'pointer';
	document.body.style.cursor = 'default';
}

DRMDrag.prototype.listMouseUp = function (row_obj)
{
	this.mouseUp();
}

DRMDrag.prototype.listMouseOut = function (row_obj)
{
	if (mouseDown)
	{
		if (ctrlDown)
		{
			ctrlDown = false;
			for (var n = 0; n < selects.length; n++)
			{
				if (selects[n] == lastClick)
					break;

			}

			// select key
			if (n == selects.length)
			{
				this.select(row_obj);
			}
		}
		shiftDown = false;
		draging = true;
		Utilities.getElement('drm_list_dsp').style.cursor = 'move';
		document.body.style.cursor = 'no-drop';
	}
}

DRMDrag.prototype.isDrag = function ()
{
	return draging;
}

var drmDrag = new DRMDrag();

DRMDrag.prototype.isDrop = function (e, folder)
{
	if (draging)
	{
		draging = false;
		TransferStart(folder);
	}
}

document.onmouseup = drmDrag.fDocMouseUp();
