function document_height() {
	if (window.innerHeight)
		return window.innerHeight;

	if (document.documentElement && document.documentElement.clientHeight)
		// Explorer 6 Strict Mode
	{
		return document.documentElement.clientHeight;
	}

	return document.body.clientHeight;
}

function document_width() {
	if (window.innerWidth)
		return window.innerWidth;

	if (document.documentElement && document.documentElement.clientWidth)
		// Explorer 6 Strict Mode
	{
		return document.documentElement.clientWidth;
	}

	return document.body.clientWidth;
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

window.onresize=resizeFrame;

function resizeFrame() {
   try {
      var doclen = document_height();

      var top = document.getElementById('TextControl').offsetTop;
      var objTextarea = getInputObj();
      doclen -= top + 30;

      if (doclen < 50)
	doclen = 50;

      objTextarea.style.height = doclen + 'px';
      oSpellCheck.adjHeight(doclen);

      //objTextarea.style.width = (document_width() - 20) + 'px';
   }
   catch (e) {
      //consoleLog(e);
      setTimeout('resizeFrame()',500);
   }
}
