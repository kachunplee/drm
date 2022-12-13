function updTzname ()
{
	var tz1obj = document.formPref.x_tz1;
	var tz = (tz1obj.selectedIndex >= 0) ?
		tz1obj.options[tz1obj.selectedIndex].value : '';

	var tz2obj = document.formPref.x_tz2;
	if (tz2obj && (tz2obj.selectedIndex >= 0))
		tz += '/' + tz2obj.options[tz2obj.selectedIndex].value;
		
	document.formPref.tzname.value = tz;
}

//
//
//
var tz2HTTP = null;
var curtz1;

function gotTZ2() 
{ 
	if (tz2HTTP.readyState==4)
	{ 
		var response = tz2HTTP.responseText;
		var html = '';
		if (response != "//")
		{
			var tzs = response.split("\n");
			html = '<select name=x_tz2 onchange=\"changeTZ2(this)\"';

			for(var n = 0; n < tzs.length; n++)
			{
				html += '<option>' + tzs[n] + '</option>';
			}
			html += '</select>';
		}

		var tz2div = document.getElementById('tz2_div');
		tz2div.innerHTML = html;
		updTzname();
	}
}

function changeTZ1 (target)
{
	var tz = (target.selectedIndex >= 0) ?
		target.options[target.selectedIndex].value : '';
	
	if (curtz1 != tz)
	{
		curtz1 = tz;

		tz2HTTP=null;
		try
		{
			// Firefox, Opera 8.0+, Safari
			tz2HTTP=new XMLHttpRequest();
		}
		catch (e)
		{
			// Internet Explorer
			try
			{
				tz2HTTP=new ActiveXObject("Msxml2.XMLHTTP");
			}
			catch (e)
			{
				tz2HTTP=new ActiveXObject("Microsoft.XMLHTTP");
			}
		}

		if (tz2HTTP==null)
		{
			alert ("Your browser does not support AJAX!");
			return;
		} 

		var url=dir_ht+"gettz2.asp";
		url += "?q=" + curtz1;
		url += "&sid=" + Math.random();
		tz2HTTP.onreadystatechange=gotTZ2;
		tz2HTTP.open("GET",url,true);
		tz2HTTP.send(null);
	}
}

function changeTZ2 (target)
{
	updTzname();
}
