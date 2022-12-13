use drmdef;

sub PopupError
{
	my ($errmsg) = @_;

	print <<EOM; 
Content-type: text/html

<head>
<title>Direct Read Mail (tm)</title>
</head>

<body link="#326698" vlink="#326698" alink="#326698" bgcolor="#EAEFF8" marginheight="0" marginwidth="0" topmargin="0" leftmargin="0">
  <div align="center">
    <table border="0" cellpadding="0" cellspacing="0" style="border-collapse: collapse" bordercolor="#111111" width="300">
        <tr>
        <td bgcolor="#3A6BB7" height=29> </td>
      </tr>
      <tr>
        <td bgcolor="#EAEFF8">
      <img border="0" src="${IMGDIR}spacer_transparent.gif" width="25" height="7"></td>
      </tr>
      <tr>
        <td bgcolor="#EAEFF8">
      <div align="center">
        <center>
        <table border="0" cellpadding="0" cellspacing="0" style="border-collapse: collapse" bordercolor="#111111" width="275">
          <tr>
            <td width="256">
      <img border="0" src="${IMGDIR}spacer_transparent.gif" width="25" height="7"></td>
          </tr>
          <tr>
            <td width="256">
      <img border="0" src="${IMGDIR}spacer_transparent.gif" width="25" height="7"></td>
          </tr>
          <tr>
            <td width="275" align="center">
            <div align="center">
              <center>
              <table border="1" cellpadding="3" cellspacing="0" style="border-collapse: collapse" bordercolor="#396BB5" width="275">
                <tr>
                  <td>
                  <font face="Arial" size="2" color="#396BB5"><b>Error: </b>
                  $errmsg</font></td>
                </tr>
              </table>
              </center>
            </div>
            </td>
          </tr>
          <tr>
            <td height="21" width="256">&nbsp;</td>
          </tr>
          <tr>
            <td width="256">
            <p align="center"><a href="javascript:history.back()"><img border="0" src="${IMGDIR}blueback.gif" width="82" height="22"></a><a href="javascript:close()"><img border="0" src="${IMGDIR}blueclose.gif" width="82" height="22"></a></td>
          </tr>
          <tr>
            <td height="20" width="256">&nbsp;</td>
          </tr>
        </table>
        </center>
      </div>
        </td>
      </tr>
      </form>
    </table>
  </div>
  <p align="center">&nbsp;</p>
</body>
</html>

EOM

exit 1;

}

1;
