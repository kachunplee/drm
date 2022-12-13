#set args 9 '<JCEILIPMODHBMEIEJPGLAEFCCAAA.joe@newsadm.com>'
#set args 1 '<4.2.0.58.20010111125541.00966d60@127.0.0.1>'
#set args 69.3 '<200102042020150840.0060D10A@mail.direct.ca>'
#set args 987987534
#set args -a 1 '<ahjv7p04sc@drn.newsadm.com>'
#set args  /in 5 '<200212302310.gBUNAlAl042771@lera.pathlink.com>'
#set args dvlhttp

#set args in -P -X 2 3C657373367276307173334064726E2E6E6577736775792E636F6D3E
#set args -X < ../test/emptytrash.d
#set args -b+in+-rbob@newsadm.com+30+3C65766C6E346D30316471644064726E2E6E6577736775792E636F6D3E
#set args -b in -rdeepinlove@sunsetbeach.co.uk 1 3C33776D36376C746F6D373366396624313332363877386B2E64656570696E6C6F76654073756E73657462656163682E636F2E756B3E
#set args -b in -rfyvdvshao@cosmos.wits.ac.za 2 3C35623361303163333439373424343262656632373024303364653732383640224D617279204869727A656C222066797664767368616F3E
#set args in -B 4 '<4wm67ltom73f9f$13268w8k.deepinlove@sunsetbeach.co.uk>'
#set args out -X -P 64
#set args in -H 1 3C5730544830353346453638433946383343434233343241373630453935302E352E343832332E70696D61696C657234322E44756D7053686F742E3140656D61696C2E63686173652E636F6D3E
#set args in
#set args tdir1/school/2nd -X130.46598576002413417
#set args out

b main
b MailBox::MailBox
b mailbox.cpp:MMakeDirP
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
#b zm_msg.cpp:ParseMultipart
#b mime_readmessage
#b MailOption::PrxPathInfo
#b mailmsg.cpp:619
#b MailMessage::PrintAttLink
#b mime_readmessage
#b mimemisc.c:179
#b MailOption::GetPOPConfig
#b mime_parseheader
#b MailTo::PrintBody
#b MailSettings::GetAlias
#b MailBox::GetCurMailInfo
#b MimeText::GetText
b Folderstat

