/* Revised: 2009-02-24 */

function document_height() {
	//if (document.innerHeight) // all except Explorer
	//	return document.innerHeight;

	if (document.documentElement.clientHeight > 0 && !window.opera)
		return document.documentElement.clientHeight;

	return document.body.clientHeight;
}

function resizePage() {
	window.onresize = null;
	try {
		if (window.resizeFrame)
		{
			startHT = 1;
			resizeFrame();
		}
	}
	catch(e) {
      		//consoleLog(e);
	}
	window.onresize = resizePage;
}

window.onresize=resizePage;

function resizeFrame( fix ) {
   try {
      var doclen = document_height();
      if (doclen < 480)
	   doclen = 480;

      var mail_frame = document.getElementById("drm_display");
      doclen -= mail_frame.offsetTop;

      var fldr_frame = document.getElementById("drm_folder");
      fldr_frame.style.height = doclen - 10 + "px";

      fldr_frame = document.getElementById("drm_folder_dsp");
      fldr_frame.style.height = doclen - 100 + "px";
      fldr_frame = document.getElementById("drm_folder_name");
      fldr_frame.style.height = doclen - 100 + "px";

      var frame;
      if (previewframe)
      {
        var frame = document.getElementById("drmPreview");
        doclen -= frame.offsetTop;
	
	// IE6 iframe bug
	if (bIE  && (parseInt( navigator.appVersion ) <= 4))
	{
        	var dsp = document.getElementById("drm_prevw_dsp");
		frame.style.width = dsp.offsetWidth + 'px';
	}
      }
      else
      {
        var frame = document.getElementById("drm_list_dsp");
	doclen -= frame.offsetTop;
      }

      doclen -= 7 + ( fix ? 1 : 0 );	// body.margin
      if( doclen < 0 )
		doclen = 0;

      frame.style.height = doclen + 'px';

      //initMailFrame(fix);
   }
   catch (e) {
      //xdebug(e);
      setTimeout('resizeFrame()',500);
   }
}

function initPreview ()
{
	resizeFrame();
}
