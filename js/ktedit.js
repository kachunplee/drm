// Copyright 2008 Newsadm, Inc.
// All Rights Reserved.

var isOpera = (navigator.appName.indexOf("Opera") != -1);
var isSafari = (navigator.appName.indexOf("Safari") != -1);
var isIE = (navigator.appName.indexOf("Internet Explorer") != -1);

var msgTime = 3000;
var oSpellCheck = new Object();

function dspSugg (e, n)
{
	oSpellCheck.showMenu(n);
}

function clickSugg (e)
{
        if (!e) var e = window.event;
	var target = (window.event) ? e.srcElement : e.target;
	oSpellCheck.changeText(target.innerHTML);
}

//
//
oSpellCheck.state = 0;
oSpellCheck.change = false;

oSpellCheck.noHover = isSafari ||
			(isIE && (parseInt(navigator.appVersion.replace(/^.*MSIE /,'')) >= 6));

oSpellCheck.getOrigWord = function (no)
    {
	return this.errors[no].split(' ')[0];
    }

oSpellCheck.markGood = function (no)
    {
	this.misspchk[no] = true;
	this.misspell[no].className = 'spellmark';
    }

oSpellCheck.changeText = function (text)
    {
	var target = this.misspell[this.nMissp];
	target.innerHTML = text;
	this.change = true;
	this.markGood(this.nMissp);
	this.nextMenu();
	return false;
    }


oSpellCheck.ignoreText = function ()
    {
	var word = this.getOrigWord(this.nMissp);
	for (var i = 0; i < this.errors.length; i++)
	{
		if (word == this.getOrigWord(i))
		{
			this.markGood(i);
		}
	}
	this.nextMenu();
	return false;
    }

oSpellCheck.replaceText = function (id)
    {
	var chg = document.getElementById(id).value;
	if (chg == '')
	{
		this.clearMenu();
		return;
	}

	chg = chg.replace(/</g, "");
	chg = chg.replace(/>/g, "");
	var nMissp = this.nMissp;
	var word = this.getOrigWord(nMissp);

	var dtarget;
	for (var i = 0; i < this.errors.length; i++)
	{
		if (word == this.getOrigWord(i))
		{
			dtarget = document.getElementById('spellerr_' + i);
			dtarget.innerHTML = chg;
			this.markGood(i);
		}
	}

	this.change = true;
	this.nextMenu();
	return false;
    }

oSpellCheck.addMenuItem = function (word, i, func)
    {
	var item = document.createElement('span');
	item.innerHTML = word;
	item.className = 'spell_sugg_item';
	if (i > 0)
	{
		item.id = 'menu_item_' + i;
		if (this.noHover)
		{
		    item.onmouseover = new Function(
			"oSpellCheck.setBG('" + item.id + "'); return true;");
		    item.onmouseout = new Function(
			"oSpellCheck.clrBG('" + item.id + "'); return true;");
		}
	}
	if (func != null)
		item.onclick = func;
	this.menu.appendChild(item);
	return item;
    }

oSpellCheck.addMenuSep = function ()
    {
	var item = document.createElement('div');
	item.innerHTML = '&nbsp';
	item.className = 'spell_sugg_sep';
	this.menu.appendChild(item);
	return item;
    }

oSpellCheck.doneToDictionary = function (response)
    {
	if (response.match(/^OK:/))
		response = response.substr(4);
	dispSpellChkMsg(response, msgTime);
	this.ignoreText();
    }

oSpellCheck.addToDictionary = function ()
    {
	var target = this.misspell[this.nMissp];
	ajaxRequest(this.input.getBinDir() + "adddct?" + target.innerHTML,
		function (response) { oSpellCheck.doneToDictionary(response); });
	return false;
    }

oSpellCheck.positionMenu = function ()
    {
	if (this.menu == null)
		return;

	var target = this.misspell[this.nMissp];
	var newtop = (this.input.getTop() + this.dsp.offsetTop
			+ target.offsetTop + target.offsetHeight);
	if (!isOpera)
		newtop -= this.dsp.scrollTop;

	if (isIE && (target.offsetLeft <= 1)
	      && (target.previousSibling != null)
		&& (target.previousSibling.nodeType == 3))
		// Yike... IE bugs...
		// offsetTop was the one line above with wrapped word.
		newtop += target.offsetHeight;

 	newtop = newtop + "px";
	this.menu.style.top = newtop;
	this.menu.style.left = (target.offsetLeft + 3) + "px";
    }

oSpellCheck.scrollMenu = function ()
{
	this.positionMenu();
	var n = this.menu.offsetTop - this.input.getTop();
	if ((n <= 0) || (n > this.dsp.offsetHeight))
		this.clearMenu();
}

oSpellCheck.createMenu = function ()
    {
	var menuContainer = document.getElementsByTagName('body')[0];
	menuContainer.appendChild(this.menu);

	var target = this.misspell[this.nMissp];
	var tTop = target.offsetTop - this.dsp.scrollTop;
	if (isOpera) tTop += this.dsp.scrollTop;
	if (tTop < 0)
	{
		// target Top above scroll region
		this.dsp.scrollTop += tTop;
	}
	else
	{
		var tHeight = target.offsetHeight + this.menu.offsetHeight;
		var over = tTop + tHeight - this.dsp.offsetHeight
		if (over > 0)
		{
		    if (tHeight > this.dsp.offsetHeight)
			over -= tHeight - this.dsp.offsetHeight;
		    this.dsp.scrollTop += over;
		}
	}

	this.positionMenu();

	var over = this.menu.offsetTop + this.menu.offsetHeight
			- this.input.getTop() - this.dsp.offsetHeight;
	if (over)
	{
		over -= document.all ?
			document.body.scrollTop : window.pageXOffset;
		if (over > 0)
			window.scrollBy(0, over); 
	}

	//this.dsp.onclick = function () {
	//		oSpellCheck.clearMenu();
	//		return false;
	//	}
	this.dsp.onscroll = function () {
			oSpellCheck.scrollMenu();
			return true;
		}
    }

oSpellCheck.replFocus = function ()
    {
	var o = document.getElementById('chgInput');
	o.focus();
    }

oSpellCheck.changeMenu = function (target)
    {
	this.clearMenu();
	this.menu = document.createElement('div');
	this.menu.id = 'spell_chg_menu';
	var imgdir = this.input.getImgDir();
	var item = this.addMenuItem('Change to: <input id=chgInput type=text ' +
	    'width=20>&nbsp;<img id=chgSubmit border=0 align=top ' +
            'src=' + imgdir + 'changeto_enter.gif width=49 height=22>&nbsp;' +
	    '<img id=chgCancel border=0 align=top ' +
	    'src=' + imgdir + 'changeto_cancel.gif width=58 height=22>' , 0);
	this.createMenu();
	var o = document.getElementById('chgSubmit');
	o.onclick = function () { oSpellCheck.replaceText('chgInput'); };
	o = document.getElementById('chgCancel');
	o.onclick = function () { oSpellCheck.clearMenu(); };
	if (isIE)
		setTimeout('oSpellCheck.replFocus()', 500);
	else
	{
		o = document.getElementById('chgInput');
		o.focus();
	}
    }

oSpellCheck.setBG = function (id)
    {
	document.getElementById(id).className = 'spell_sugg_hover';
    }

oSpellCheck.clrBG = function (id)
    {
	document.getElementById(id).className = 'spell_sugg_item';

	//this.menu.getElementsByTagName('span')[nspan].className = 'spell_sugg_hover';
    }

oSpellCheck.showMenu = function (n)
    {
	this.clearMenu();

	target = this.misspell[n];
	var wd = this.errors[n].split(' ');
	
	this.nMissp = n;
	this.menu = document.createElement('div');
	this.menu.id = 'spell_sugg_menu';

	var item;
	var id_off = 1;
	item = this.addMenuItem('Ignore', id_off++,
		function () {
			return oSpellCheck.ignoreText();
		});
	item = this.addMenuItem('Cancel', id_off++,
		function () {
			oSpellCheck.clearMenu();
			return false;
		});
	this.addMenuSep();
	if (this.input.isDct())
	    item = this.addMenuItem( 'Add to dictionary...', id_off++,
		function () { return oSpellCheck.addToDictionary(); });

	item = this.addMenuItem('Change to...', id_off++,
		function () {
			oSpellCheck.changeMenu(target);
			return false;
		});
	this.addMenuSep();
	if (wd.length <= 1)
	{
		this.addMenuItem(null, id_off++, '(no suggestions)');
	}
	else
	     for (var i = 1; i < wd.length && i < 10; i++) {
		var word = wd[i];
		var item = this.addMenuItem(word, id_off++, clickSugg);
	     }

	this.createMenu();
    }

oSpellCheck.clearMenu = function ()
    {
	if (this.menu != null) {
		//this.dsp.onclick = null;
		this.dsp.onscroll = null;
		this.menu.parentNode.removeChild(this.menu);
		this.menu = null;
	}
    }

oSpellCheck.nextMenu = function ()
    {
	// Loop to mark all other 
	var nMissp = this.nMissp;

	for(var n = nMissp + 1; ; n++)
	{
		if (n >= this.misspell.length)
			n = 0;

		if (n == nMissp)
			break;

		if (!this.misspchk[n])
		{
			this.showMenu(n);
			return;
		}
	}

	// No more miss
	this.clearMenu();
	dispSpellChkMsg ('No More Misspelled Word', msgTime);
    }

oSpellCheck.save = function ()
    {
	if (this.change == false)
		return;

	var post = this.dsp.innerHTML;
	var idx;
	var newstg = '';
	var stg = post.replace(/<\/span>/ig, '');
	while((idx = stg.indexOf("<span ")) >= 0)
	{
		newstg += stg.substring(0, idx);
		stg = stg.substring(idx);
		stg = stg.substring(stg.indexOf(">")+1);
	}
	post = newstg + stg;

	// Discard the 3 end lines added by spell check
	this.input.putValue(post.replace(/<br><br><br>$/,''));
	this.change = false;
    }

oSpellCheck.done = function ()
    {
	if (this.state >= 1)
	{
		this.clearMenu();
		this.save();
		this.dsp.style.visibility = 'hidden';
		this.input.visible();
		this.state = 0;
	}
    }

oSpellCheck.check = function (response)
    {
	if (this.state != 1)
		return;

	this.misspell = new Array();
	this.misspchk = new Array();
	this.nMissp = 0;

	if (response.indexOf('\n') == 0)
	{
		dispSpellChkMsg("No Misspelled Words", msgTime);
		this.done();
		return;
	}

	var n = response.indexOf('\n\n');

	this.errors = response.substr(0, n).split('\n');
	// Add 3 line to make room for [Change To] menu
	this.dsp.innerHTML = response.substr(n+2) + "<br><br><br>";

	var span = this.dsp.getElementsByTagName('SPAN');
	var firsterr;
	for(var i = 0; i < span.length; i++)
	{
		if(span[i].className == 'spellerror')
		{
			this.misspell.push(span[i]);
			this.misspchk.push(false);
			//span[i].onmouseover = new Function('e', 'dspSugg(e, ' + i + '); return false;');
			span[i].onclick = new Function('e', 'dspSugg(e, ' + i + '); return false;');
		}
	}

	this.showMenu(0);
	this.state = 2;
    }

oSpellCheck.start = function (oIn)
    {
	if (this.state > 0)
	{
		this.done();
		return;
	}

	this.input = oIn;

	this.spellChecker = this.input.spellChecker;

	if (!this.input.getValue().match(/[^\n\r\t ]/))
	{
		dispSpellChkMsg("Empty document", msgTime);
		return;
	}

	if (this.dsp == null)
		this.dsp = document.getElementById("TextSpell");

	this.state = 1;

	this.dsp.style.width = this.input.getWidth() - 6 + 'px';
	this.dsp.style.height = this.input.getHeight() + 'px';
	this.dsp.style.fontFamily = this.input.getFontFamily();
	this.dsp.style.fontSize = this.input.getFontSize();

	this.dsp.innerHTML = '<div align=center><div style="margin: 100px 20% 0 20%; padding: 1em; border: solid 1px #00008B;">Running spell check. Please wait...</div></div>';
	this.input.hide();
	this.dsp.style.visibility = 'visible';

	ajaxRequest(this.spellChecker,
		function(r) { oSpellCheck.check(r); }, this.input.getValue());
    }

oSpellCheck.adjHeight = function (height)
    {
	if (this.dsp)
	{
		if (this.dsp.offsetHeight != height)
		{
			this.dsp.style.height = height + 'px';
			if (this.state == 2) this.scrollMenu();
		}
	}
    }


var spellchkmsgTimeout = -1;

function clearSpellChkMsgTimeout ()
{
	var t = spellchkmsgTimeout;
	spellchkmsgTimeout = -1;
	if (t != -1)
		clearTimeout(t);
}

function setSpellChkMsgTimeout (timeout)
{
	clearSpellChkMsgTimeout();
	spellchkmsgTimeout = setTimeout('hideSpellChkMsg()', timeout);
}

function dispSpellChkMsg (msg, timeout)
{
	oSpellCheck.input.outMsg(msg);
	if (timeout)
		setSpellChkMsgTimeout(timeout);
}

function hideSpellChkMsg ()
{
	clearSpellChkMsgTimeout();

	oSpellCheck.input.hideMsg();
}
