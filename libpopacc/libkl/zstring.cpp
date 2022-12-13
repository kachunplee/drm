#include "zstring.h"

istream &
operator>> (istream &is, zstring &s)
{
  int w = is.width (0);
  s.resize (0);
  if (is.ipfx0 ())
    {
      register streambuf *sb = is.rdbuf ();
      while (1)
	{
	  int ch = sb->sbumpc ();
	  if (ch == EOF)
	    {
	      is.setstate (ios::eofbit);
	      break;
	    }
	  else if (ch == '\n')
	    {
	      sb->sungetc ();
	      break;
	    }
	  s += ch;
	  if (--w == 1)
	    break;
	}
    }

  is.isfx ();
  if (s.length () == 0)
    is.setstate (ios::failbit);

  return is;
}
