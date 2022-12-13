NumberUtil = {};

NumberUtil.getParity = function(num)
{
	if(num % 2 == 0)
	{
		return "even";
	}
	else
	{
		return "odd";
	}
}