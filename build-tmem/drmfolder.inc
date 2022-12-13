<div id="drm_folder_title" class="drm_title">
<a href="javascript:NewFolder()" title="Add New Folder"><img src="/drm/img/addfolder.gif" border="0" title="Add New Folder" width="31" height="18"></a>
<a href="javascript:EmptyTrash()" title="Empty Trash"><img border="0" src="/drm/img/emptytrash.gif" title="Empty Trash" width="29" height=18></a>
<a href="javascript:EmptyInBox()" title="Empty In Box"><img border="0" src="/drm/img/emptyinbox.gif" title="Empty In Box" width="31" height=18></a>
</div>
<div id="drm_folder_dsp">
<div id="drm_folder_name">
<script>
<!--
	for(i = 0; i < BFolders.length; i++)
		OutFolder(BFolders[i], 'B');

	for(i = 0; i < frame.Folders.length; i++)
		OutFolder(Folders[i], '');

	SetFolderImg(currentFolder);

	var menuName = "BFolder";
	BeginMenu(menuName, 90, "#6A97D9");
		MenuItem(menuName, "Open Folder", "Open Folder",
			"javascript:OpenFolder()");
		MenuItem(menuName, "Transfer Selected", "Transfer Selected",
			"javascript:TransferSelected()");
	EndMenu();

	var menuName = "Folder";
	BeginMenu(menuName, 90, "#6A97D9");
		MenuItem(menuName, "Open Folder", "Open Folder",
			"javascript:OpenFolder()");
		MenuItem(menuName, "Transfer Selected", "Transfer Selected",
			"javascript:TransferSelected()");
		MenuItem(menuName, "Rename Folder", "Rename...",
			"javascript:RenameFolder()");
		MenuItem(menuName, "Delete Folder", "Delete...",
			"javascript:DeleteFolder()");
	EndMenu();
//-->
</script>
</div>
<script>
<!--
	OutStorageBar();
//-->
</script>
</div>
