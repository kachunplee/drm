function ajaxRequest(url, handle, postdata)
{
  var httpRequest;
  try
  {
    httpRequest=new XMLHttpRequest();
  }
  catch (e)
  {
    // Internet Explorer
    try
    {
      httpRequest=new ActiveXObject("Msxml2.XMLHTTP");
    }
    catch (e)
    {
      try
      {
        httpRequest=new ActiveXObject("Microsoft.XMLHTTP");
      }
      catch (e)
      {
        return false;
      }
    }
  }

  httpRequest.onreadystatechange = function()
      {
        if(httpRequest.readyState==4)
        {
		handle(httpRequest.responseText);
        }
      }

  if (postdata)
  {
    httpRequest.open("POST",url,true);
    httpRequest.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    httpRequest.setRequestHeader("Content-length", postdata.length);
    httpRequest.setRequestHeader("Connection", "close");
    httpRequest.send(postdata);
  }
  else
  {
    httpRequest.open("GET",url,true);
    httpRequest.send(null);
  }

  return true;
}
