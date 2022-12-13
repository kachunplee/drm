/************************************************************************
Copyright Notice

Copyright (c) MCNC, Clearinghouse for Networked Information Discovery and
Retrieval, 1994. 

Permission to use, copy, modify, distribute, and sell this software and
its documentation, in whole or in part, for any purpose is hereby granted
without fee, provided that

1. The above copyright notice and this permission notice appear in all
copies of the software and related documentation. Notices of copyright
and/or attribution which appear at the beginning of any file included in
this distribution must remain intact. 

2. Users of this software agree to make their best efforts (a) to return
to MCNC any improvements or extensions that they make, so that these may
be included in future releases; and (b) to inform MCNC/CNIDR of noteworthy
uses of this software. 

3. The names of MCNC and Clearinghouse for Networked Information Discovery
and Retrieval may not be used in any advertising or publicity relating to
the software without the specific, prior written permission of MCNC/CNIDR. 

THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,
EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY WARRANTY
OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE. 

IN NO EVENT SHALL MCNC/CNIDR BE LIABLE FOR ANY SPECIAL, INCIDENTAL,
INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES WHATSOEVER
RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT ADVISED OF THE
POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY, ARISING OUT OF OR
IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE. 
************************************************************************/

#ifndef _cgi_
#define _cgi_

#include "gdt.h"

#define CGI_MAXENTRIES 1024

typedef struct tagENTRY {
        char   *name;
        char   *val;
} ENTRY;
typedef struct tagENTRY *PENTRY;

typedef struct tagCGI {
	struct tagENTRY Entries[CGI_MAXENTRIES];	
	INT4 EntryCount;
} CGI;
typedef CGI *PCGI;

PCGI cgi_Create();
void cgi_Destroy(PCGI cgi);
void cgi_GetInput(PCGI cgi);
PCHR cgi_GetName(PCGI cgi, INT4 i);
PCHR cgi_GetValue(PCGI cgi, INT4 i);
PCHR cgi_GetValueByName(PCGI cgi, PCHR Name);
void cgi_DisplayContents(PCGI cgi);

#endif
