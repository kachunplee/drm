var bIEMac = ((navigator.appName.indexOf("Internet Explorer") != -1) &&
                (navigator.userAgent.indexOf("Mac") != -1));
var bDHML = (parseInt( navigator.appVersion ) >= 4);

function DspStatus (msg) 
{
	top.status = msg;
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
			document.captureEvents(Event.MOUSEOVER|Event.MOUSEOUT|Event.BLUR);
		}
	}
}

function BeginMenu (mn, w, bg)
{
	if (document.layers)
		document.writeln('<layer class=cFloatMenu id=divMenu', mn,
			' bgcolor=', bg, ' top=0 left=0 width=', w,
			' visibility=hide>');
	else
		document.writeln('<div class=cFloatMenu id=divMenu', mn,
			' style="width:', w,
			';position:absolute;z-index:1;left:0;visibility:hidden;">');
}

function EndMenu ()
{
	if (document.layers)
		document.writeln('<p></layer>');
	else
		document.writeln('</div>');
}

function MenuHr ()
{
	document.write('<hr width="100%" size="1" color="black">');
}

function MenuItem (name, msg, txt, url)
{
	document.write('<a href=', url,
		' onMouseOver="return DspStatus(\'', msg, '\')"',
		' onMouseOut="return DspStatus(\'\')">', txt, '</a><br>');
}

function CloseMenu (menu)
{
	if (document.layers)
	{
		menu.visibility = "hide";
	}
	else
	{
		menu.style.visibility = "hidden";
	}

	openedMenu = null;
}


function OpenMenu (obj, menu)
{
	if (openedMenu && openedMenu != menu) 
		CloseMenu(openedMenu);

	if (menu)
	{
		if (document.layers)
		{
			menu.moveTo(menuX(obj.x, obj.width),
					menuY(obj.y, obj.height));
			menu.visibility = "show";
			menu.align = "left";
		}
		else
		{
			var x = obj.offsetLeft;
			var y = obj.offsetTop;
			for (var o = obj;
				o.offsetParent && obj != o.offsetParent;
					o = o.offsetParent)
			{
				x += o.offsetParent.offsetLeft;
				y += o.offsetParent.offsetTop;
			}
			menu.style.left = menuX(x, obj.offsetWidth);
			menu.style.top = menuY(y, obj.offsetHeight);
			menu.style.visibility = "visible";
			menu.align = "left";
		}

		openedMenu = menu;
	}
}

function CloseOpenedMenu ()
{
	CloseMenu(openedMenu);
	openedMenu = null;
}

function mouseover (evt)
{
	if (!bDHML)
		return;
	var obj = evt ?  evt.target : window.event.srcElement;

	var id = document.layers ? (obj.name ? obj.name : '') : obj.id;
	var item = id;
	if (id) id = id.replace(/_.*$/, "");
	DspStatus(id);
	if (!id || id.indexOf("iMenu") != 0)
	{
		if (openedMenu)
		{
			if (document.layers)
			{
				if (openedMenu == obj)
					return;

				if (obj.parentLayer &&
					openedMenu == obj.parentLayer)
					return;

				var i;
				for (i = openedMenu.document.links.length-1; i>=0; i--)
					if (openedMenu.document.links[i] == obj)
						return;
			}
			else if (document.all)
			{
				if (openedMenu.contains(obj))
					return;
			}
			else
			{
				//if (openedMenu == obj
				//	|| openedMenu == obj.parentNode
				//	|| openedMenu == obj.parentNode.parentNode)
				//return;

				var node;
				for (node = obj; node; node = node.parentNode)
				{
					if (openedMenu == node)
						return;
				}
			}

			CloseMenu(openedMenu);
		}
		return;
	}

	nameItem = item.replace(/^.*_/,'');
	DspStatus(nameItem);
	id = id.replace("iMenu", "divMenu");

	var menu;
	if (document.layers)
	{
		menu = document.layers[id];
	}
	else if (document.getElementById)
	{
		menu = document.getElementById(id);
	}
	else if (document.all)
	{
		menu = document.all[id];
	}
	else return;

	OpenMenu(obj, menu);
}
