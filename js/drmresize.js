function initMailFrame (fix)
{
        var mail_frame = document.getElementById("drm_display");
	var h = document_height() -
      		mail_frame.offsetTop + ( fix ? 1 : 0 ) - 10;
	if (h < 270)
		h = 270;
	mail_frame.style.height = h + "px";
        var fldr_frame = document.getElementById("drm_folder");
	fldr_frame.style.height = h + "px";
}
