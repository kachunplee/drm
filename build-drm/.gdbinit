#set args 9 '<JCEILIPMODHBMEIEJPGLAEFCCAAA.joe@newsguy.com>'
#set args 1 '<4.2.0.58.20010111125541.00966d60@127.0.0.1>'
#set args 69.3 '<200102042020150840.0060D10A@mail.direct.ca>'
#set args 987987534
#set args -a 1 '<ahjv7p04sc@drn.newsguy.com>'
#set args  /in 5 '<200212302310.gBUNAlAl042771@lera.pathlink.com>'
#set args dvlhttp

#set args in -P -X 2 3C657373367276307173334064726E2E6E6577736775792E636F6D3E
#set args -X < ../test/emptytrash.d
#set args -b+in+-rbob@newsguy.com+30+3C65766C6E346D30316471644064726E2E6E6577736775792E636F6D3E

set args 2005

b main
b UpdateFolderstat
#b MailMessage::OutBegin
#b MailOpt::SetCmd

#b MailBox::SaveIndex
#b MailBox::GetNextMail
#b MailList::PrintLine
#b mailbox.cpp:557
#b MailMessage::PrintBody
#b MailMessage::PrintAttLink
#b ZMIME::IsPlain
#b ZMIME::IsHTML
#b ZMIME::DecodeBody
#b ZMIME::ParseMultipart
#b mime_readmessage
#b MailOption::PrxPathInfo
#b mailmsg.cpp:619
#b MailMessage::PrintAttLink
#b mime_readmessage
#b mimemisc.c:179
#b MailOption::GetPOPConfig
#b mime_parseheader
