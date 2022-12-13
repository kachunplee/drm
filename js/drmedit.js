var oInput = new Object();

oInput.spellChecker = '/drm-bin/drmspell' + (isHTML?'?-html':'');

oInput.isDct = function ()
    {
	return true;
    }

oInput.getValue = function ()
    {
	//return "Hte quikc borwn fox jumps over the layz dog";
	var obj = document;
	var oFrame = frames['rte1'];
	var oInput = document.getElementById("TextBody");
	if (oFrame)
		oInput.value = oFrame.document.body.innerHTML;

	var value = oInput.value;
	if (isHTML)
	{
		var idx;
		idx = value.indexOf("<!-- DRM QUOTE -->");
		if (idx >= 0)
		{
			this.quote = value.substring(idx);
			value = value.substring(0, idx);
		}
	}
	return value;
    }

oInput.putValue = function (value)
    {
	if (isHTML)
	{
		idx = value.indexOf("<!-- DRM QUOTE -->");
		if (this.quote != null)
			value += this.quote;
	}
	else
	{
		// For DRN
		value = value.replace(/<br>/ig, "\n")
			.replace(/<[^>]*>/g, '')
			.replace(/&lt;/g, '<')
			.replace(/&gt;/g, '>')
			.replace(/&nbsp;/g, ' ')
			.replace(/&amp;/g, '&');
	}
	var obj = document;
	var oFrame = frames['rte1'];
	if (oFrame)
		oFrame.document.body.innerHTML = value;

	var oInput = document.getElementById("TextBody");
	oInput.value = value;
    }

oInput.getTop = function ()
    {
	if (this.control == null)
		this.control = document.getElementById('TextControl');
	return this.control.offsetTop;
    }

oInput.getWidth = function ()
    {
	return this.obj.offsetWidth;
    }

oInput.getHeight = function ()
    {
	return this.obj.offsetHeight;
    }

oInput.getFontFamily = function ()
    {
	return this.obj.style.fontFamily;
    }

oInput.getFontSize = function ()
    {
	return this.obj.style.fontSize;
    }

oInput.hide = function ()
    {
	//if (this.miniBar == null)
	//	this.miniBar = document.getElementById("miniBar");
	//this.miniBar.src = '/drn/drnposting_minibar_done.gif';
	var obj = document.getElementById("spellchklnk");
	obj.innerHTML = "Spell Done";
	this.obj.style.visibility = 'hidden';
	obj = document.getElementById("sendmessage");
	obj.style.color = '#C0C0C0';
    }

oInput.visible = function ()
    {
	this.obj.style.visibility = 'visible';
	var obj = document.getElementById("spellchklnk");
	obj.innerHTML = "Spell Check";
	//this.miniBar.src = '/drn/drnposting_minibar.gif';
	obj = document.getElementById("sendmessage");
	obj.style.color = '#FFFFFF';
    }

oInput.outMsg = function (msg)
    {
	var obj = document.getElementById("spellchkmsg");
	obj.innerHTML = msg;
	obj.style.visibility = 'visible';
    }

oInput.hideMsg = function ()
    {
	var obj = document.getElementById("spellchkmsg");
	obj.style.visibility = 'hidden';
	obj.innerHTML = '';
    }

oInput.getImgDir = function ()
    {
	// retrun '/drn/';
	return DIR_IMG;
    }

oInput.getBinDir = function ()
    {
	// return '/news-bin/';
	return DIR_BIN;
    }

function getInputObj ()
    {
	return document.getElementById(isHTML ? "rte1" : "TextBody");
    }

function startSpell ()
{
	// if(DOM not supported) { SpellCheck(); return; }

	if (oInput.obj == null)
		oInput.obj = getInputObj();
	oSpellCheck.start(oInput);
}

function initSpell ()
{
	if (isOpera)
        {
                var o = isHTML ? document.getElementById('rte1')
                		: document.getElementById('TextInput')
		if (o)
                	o.style.position = 'relative';
        }
	resizeFrame();
}
