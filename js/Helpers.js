Utilities = {};

Utilities.includeJS = function(filepaths)
{
	for(var i=0; i<filepaths.length; i++)
	{
		document.write('<script type="text/javascript" src="'+filepaths[i]+'"></script>');
	}
}

Utilities.includeCSS = function(filepaths)
{
	for(var i=0; i<filepaths.length; i++)
	{
		document.write('<link href="'+filepaths[i]+'" rel="stylesheet" type="text/css" />');
	}
}

Utilities.getElement = function(i) { return document.getElementById(i); }

Utilities.debug = function(val)
{
	this.getElement('debug').innerHTML += val +"<br/>";
}

Utilities.toggle = function(id)
{
	this.getElement(id).style.display = (this.getElement(id).style.display == '') ? 'none' : '';
}

Utilities.createElement = function(e, obj)
{
	var a = document.createElement(e);
	for(prop in obj)
	{
		a[prop] = obj[prop];
	}
	return a;
}

Utilities.appendChild = function()
{
	if(this.appendChild.arguments.length > 1)
	{
		var a = this.appendChild.arguments[0];
		for(i=1; i<this.appendChild.arguments.length; i++)
		{
			if(arguments[i])
			{
				a.appendChild(this.appendChild.arguments[i]);
			}
		}
		return a;
	}
	else
	{
		return null;
	}
}

Utilities.removeChildren = function(node)
{
	if(node == null)
	{
		return;
	}
	
	while(node.hasChildNodes())
	{
		node.removeChild(node.firstChild);
	}
}

Utilities.addListener = function(obj, eventName, listener)
{
	if (obj.attachEvent)
	{
		obj.attachEvent("on"+eventName, listener);
	}
	else if(obj.addEventListener)
	{
		obj.addEventListener(eventName, listener, false);
	}
	else
	{
		return false;
	}
	return true;
}

Utilities.removeListener = function(obj, eventName, listener)
{
	if(obj.detachEvent)
	{
		obj.detachEvent("on"+eventName, listener);
	}
	else if(obj.removeEventListener)
	{
		obj.removeEventListener(eventName, listener, false);
	}
	else
	{
		return false;
	}
	
	return true;
}

Utilities.changeOpac = function(opacity, id)
{
	var object = Utils.ge(id).style;
    object.opacity = (opacity / 100);
    object.MozOpacity = (opacity / 100);
    object.KhtmlOpacity = (opacity / 100);
    object.filter = "alpha(opacity=" + opacity + ")";
}
