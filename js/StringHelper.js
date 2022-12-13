StringUtil = {};

StringUtil.trim = function(val, length)
{
	if(val != undefined)
	{
		val = (val.length >= length) ? val.substring(0, length) + '...' : val;
		return val;
	}
	else
	{
		return "";
	}
}