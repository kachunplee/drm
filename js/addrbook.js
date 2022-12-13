function menuX (x, width)
{
	return (x + width/5);
}

function menuY (y, height)
{
	return (y + height);
}

function EditContact (mtime)
{
	CloseOpenedMenu();
	location.href = '/drm-bin/frmaddbked?' + mtime + "+" + nameItem;
}

function DeleteContact (mtime)
{
	CloseOpenedMenu();
	location.href = '/drm-bin/frmaddbkmn?-d' + mtime + "+" + nameItem;
}
