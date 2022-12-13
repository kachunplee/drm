var xmlHttp;
var addrs;
var getaddstr;

var suggestfld;

function showSuggestion(dir_ht, str, fldname)
{
	if (fldname.length==0)
		suggestfld = 'Tofield';
	else
		suggestfld = fldname;
	if (str.length==0)
	{ 
		var div = document.getElementById('suggestDiv');
		div.innerHTML="";
		div.style.display = 'none';
		return;
	}
	xmlHttp=GetXmlHttpObject()
	if (xmlHttp==null)
	{
		alert ("Your browser does not support AJAX!");
		return;
	} 

	getaddstr = str;
	var url=dir_ht+"getaddbk.asp";
	url=url+"?q="+str;
	url=url+"&sid="+Math.random();
	xmlHttp.onreadystatechange=stateChanged;
	xmlHttp.open("GET",url,true);
	xmlHttp.send(null);
} 

//
//
//
function hideSuggestion()
{
	var div = document.getElementById('suggestDiv');
	div.style.display = 'none';
}

function selectChoice (select)
{
	var to = document.getElementById(suggestfld);
	var options = select.getElementsByTagName('option');
	for (var n = 0; n < options.length; n++)
	{
		if (options[n].selected)
		{
			var tostg = to.value;
			if (tostg == '')
			{
				// tostg = options[n].text;
				if(addrs[n] == '')
					tostg = options[n].text;
				else
					tostg = unescape(addrs[n]);
			}
			else
			{
				if(addrs[n] == '')
					tostg = tostg + ", " + options[n].text;
				else
					tostg = tostg + ", " + unescape(addrs[n]);
			}
		
			to.value = tostg;
			hideSuggestion();
			return;
		}
	}
}

function selectClick (e)
{
	var addaddr = e.innerHTML;
	var div = document.getElementById('suggestDiv');
	var n = 0;
	var elm;
	for(var i = 0; i < div.childNodes.length; i++)
	{
		elm = div.childNodes[i];
		if (e.innerHTML == elm.innerHTML)
		{
			if (addrs[i] != '')
			{
				addaddr = unescape(addrs[i]);
				addaddr = addaddr.replace(/\&lt;/g, '<');
				addaddr = addaddr.replace(/\&gt;/g, '>');
				addaddr = addaddr.replace(/\&amp;/g, '&');
				addaddr = addaddr.replace(/\&quot;/g, '"');
				addaddr = addaddr.replace(/\&#39;/g, "'");
			}
			break;
		}
	}

	var to = document.getElementById(suggestfld);
	var tostg = to.value;
	if (tostg == '' || getaddstr != '*')
		tostg = addaddr;
	else
		tostg = tostg + ", " + addaddr;
	to.value = tostg;
	hideSuggestion();
}

function posX (e)
{
	return e.offsetParent ? e.offsetLeft + posX(e.offsetParent) :
		e.offsetLeft;
}

function posY (e)
{
	return e.offsetParent ? e.offsetTop + posY(e.offsetParent) :
		e.offsetTop;
}

//
//
//
function stateChanged() 
{ 
	if (xmlHttp.readyState==4)
	{ 
		var reply = xmlHttp.responseText;

		var div = document.getElementById('suggestDiv');
		if (reply == ',,no,,')
		{
			div.innerHTML = '';
			div.style.display = 'none';
			addrs.clear();
			getaddtype = '';
			return;
		}

		var res = reply.split("\n");
		var hints = res[0].split(", ");
		addrs = res[1].split(", ");

		var html = ''
		for(var n = 0; n < hints.length; n++)
		{
			html += '<div id="suggestEntry" onclick="selectClick(this)">' + hints[n] + '</div>';
		}
		html += '';

		div.innerHTML = html;

		// place div
		var obj = document.getElementById(suggestfld);
		div.style.top = (posY(obj) + obj.offsetHeight-3) + 'px';
		div.style.left = posX(obj) + 'px';
		div.style.display = 'block';
	}
}

function GetXmlHttpObject()
{
	var xmlHttp=null;
	try
	{
		// Firefox, Opera 8.0+, Safari
		xmlHttp=new XMLHttpRequest();
	}
	catch (e)
	{
		// Internet Explorer
		try
		{
			xmlHttp=new ActiveXObject("Msxml2.XMLHTTP");
		}
		catch (e)
		{
			xmlHttp=new ActiveXObject("Microsoft.XMLHTTP");
		}
	}
	return xmlHttp;
}

function SendMessage ()
{
	if (oSpellCheck && (oSpellCheck.state > 0))
		return;

	if (document.PostForm.mailto.value == '')
	{
		OpenErrorWin('mailto');
		return;
	}
	if (document.PostForm.from.value == '')
	{
		OpenErrorWin('from');
		return;
	}
	var obj = document;
	var oInput = document.getElementById("TextBody");
	var oFrame = frames['rte1'];
	if (oFrame)
	{
		var oRTE = oFrame.document;
		document.PostForm.MailHTML.value = 'yes';
		oInput.value = oFrame.document.body.innerHTML;
	}
	document.PostForm.MailBody.value = oInput.value;
	document.PostForm.submit();
}

function displayRow (obj)
{
	do
	{
		obj = obj.parentNode;
                if (obj == null)
                        return;
        }
        while(obj.tagName != 'TR');
	
	obj.style.display = '';
}

function dspFileName (attfiles, filename)
{
	var n = filename.lastIndexOf('/');
	if (n > 0)
		filename = filename.substr(n+1);

	n = filename.lastIndexOf('\\\\');
	if (n > 0)
		filename = filename.substr(n+1);

	var fileText = document.createTextNode(filename + '; ');
	attfiles.appendChild(fileText);
	displayRow(attfiles);

	//var choice = document.getElementById('multiChoice');
	//choice.style.display = '';
	
}

function setform ()
{
        var attfiles = document.getElementById('attFiles');

	for(var tr = document.getElementById('attuploads');
		tr != null;
		tr = tr.nextSibling)
	{
		if (tr.tagName != 'TR')
			continue;

		var cell;
		var n = 0;
		for(var i = 0; i < tr.childNodes.length; i++)
		{
			cell = tr.childNodes[i];
			if (cell.tagName == 'TD')
			{
				if (++n == 3)
					break;
			}
		}

		var upload;
		for(var i = 0; i < cell.childNodes.length; i++)
		{
			upload = cell.childNodes[i];
			if (upload.tagName == 'INPUT')
				break;
		}

		if (upload.value == '')
		{
			tr.style.display = '';
			break;
		}

		tr.style.display = 'none';

		dspFileName(attfiles, upload.value);
	}

	//resizeFrame();
}

function gotFile (obj)
{
	var filename = obj.value;

       	// Find TR
	do
	{
		obj = obj.parentNode;
		if (obj == null)
			return;
	}
	while(obj.tagName != 'TR');

	// Turn off file upload line
	obj.style.display = 'none';

	// Find next file upload
	do
	{
		obj = obj.nextSibling;
		if (obj == null)
			return;
	}
	while(obj.tagName != 'TR');

	// Turn it on
	obj.style.display = '';

	dspFileName(document.getElementById('attFiles'), filename);
}
