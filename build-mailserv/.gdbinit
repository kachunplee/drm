#set args 9 '<JCEILIPMODHBMEIEJPGLAEFCCAAA.joe@newsadm.com>'
#set args 1 '<4.2.0.58.20010111125541.00966d60@127.0.0.1>'
#set args 69.3 '<200102042020150840.0060D10A@mail.direct.ca>'
#set args 987987534
#set args -a 1 '<ahjv7p04sc@drn.newsadm.com>'
#set args -p1
#set args in -r mike@newsadm.com 5 '<200212302310.gBUNAlAl042771@lera.pathlink.com>'
set args perry
set args in -a 1 3C586E73393933333732333036374641396C75757472616E67656F6369746965734036342E3230392E302E39323E


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
#b in.cpp:67
#b mime_parseheader
b main
