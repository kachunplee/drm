var Menus = new Array();
var timeoutID = -1;

if (!document.getElementById)
    document.getElementById = function() { return null; }

function closeMenu (level)
{
	for (var i = level; i < Menus.length; i++)
	{
        	if (Menus[i]) {
            		Menus[i].style.visibility = "hidden";
			Menus[i] = null;
        	}
	}
}

function clearMenu ()
{
	if (timeoutID == -1)
		return;

	closeMenu(0);
}

function clearMenuTimeout ()
{
	var t = timeoutID;
	timeoutID = -1;
	if (t != -1)
		clearTimeout(t);
}

function setMenuTimeout ()
{
	if (clearMenu)
		timeoutID = setTimeout('clearMenu()', 250);
}

function closeChildMenu (level)
{
	clearMenuTimeout();
	closeMenu(level+1);
}

function initializeMenu (menuId, selectId, level, xoffset) {
    var menu = document.getElementById(menuId);
    if (menu == null)
	return;

    var obj;
    for (var i = 0; menu.childNodes[i]; i++)
    {
	obj = menu.childNodes[i];
	if (obj.tagName == 'LI')
	{
	    if (obj.className != 'menubar')
		obj.onmouseover = function() {
			closeChildMenu(level);
		}
	    else
		obj.onmouseover = clearMenuTimeout;
	    obj.onmouseout=setMenuTimeout;
	}
	obj = obj.nextSibling;
    }

    var selector = document.getElementById(selectId);
    if (selector == null)
	return;

    if (level < 0)
	return;

    selector.onmouseover = function() {
	clearMenuTimeout();

        if (Menus[level]) {
            Menus[level].style.visibility = "hidden";
        }
	
	closeMenu(level+1);
        this.showMenu();
    }

    selector.onmouseout = setMenuTimeout;

    selector.showMenu = function() {
      if (level == 0)
      {
	var x = this.offsetLeft;
	var y = this.offsetTop;
	for (var o = this; o.offsetParent && this != o.offsetParent;
			o = o.offsetParent)
        {
                x += o.offsetParent.offsetLeft;
                y += o.offsetParent.offsetTop;
        }
        menu.style.left = (x + xoffset) + "px";
        menu.style.top = (y + this.offsetHeight - 2) + "px";
      }
      else
      {
        menu.style.left = (this.offsetLeft + this.offsetWidth) + "px";
        menu.style.top = (this.offsetTop + 2) + "px";
      }
      menu.style.visibility = "visible";
      menu.style.zIndex = 100;
      Menus[level] = menu;
    }
}
