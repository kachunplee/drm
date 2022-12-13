//
//
//
AjaxUpdater = {};

AjaxUpdater.initialize = function()
{
	AjaxUpdater.isUpdating = false;
}

AjaxUpdater.initialize();

AjaxUpdater.Update = function(method , service, callback, postdata)
{
	if(callback == undefined || callback == "") { callback = AjaxUpdater.onResponse; }
	Ajax.makeRequest(method, service, callback, postdata);
	AjaxUpdater.isUpdating = true;
}

AjaxUpdater.onResponse = function()
{
	if(Ajax.checkReadyState('loading') == "OK")
	{
		AjaxUpdater.isUpdating = false;
	}
}

//
//
//
Ajax = {};

Ajax.makeRequest = function(method, url, callbackMethod, postdata)
{
	this.request = (window.XMLHttpRequest)? new XMLHttpRequest(): new ActiveXObject("MSXML2.XMLHTTP"); 
	this.request.onreadystatechange = callbackMethod;
	
	this.request.open(method, url, true);
	if (postdata)
	{
		this.request.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
		this.request.setRequestHeader("Content-length", postdata.length);
		this.request.setRequestHeader("Connection", "close");
		this.request.send(postdata);
	}

	this.request.send(url);
}
	
Ajax.checkReadyState = function(_id)
{
	switch(this.request.readyState)
	{
		case 1:
			document.getElementById(_id).innerHTML = 'Loading ...';
			break;
		case 2:
			document.getElementById(_id).innerHTML = 'Loading ...';
			break;
		case 3:
			document.getElementById(_id).innerHTML = 'Loading ...';
			break;
		case 4:
			AjaxUpdater.isUpdating = false;
			document.getElementById(_id).innerHTML = '';
			return HTTP.status(this.request.status);
		default:
			document.getElementById(_id).innerHTML = "An unexpected error has occurred.";
	}
}
	
Ajax.getResponse = function()
{
	if(this.request.getResponseHeader('Content-Type').indexOf('xml') != -1)
	{
		return this.request.responseXML.documentElement;
	}
	else
	{
		return this.request.responseText;
	}
}
