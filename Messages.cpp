#include "Messages.h"
#include "Portal.h"
#include "utils/LineEdit.h"

#include <htmlcxx/html/ParserDom.h>
#include <htmlcxx/html/CharsetConverter.h>
#include <boost/lexical_cast.hpp>


#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <fcntl.h>

#include <Wt/Mail/Client>
#include <Wt/Mail/Message>


#include <Wt/WApplication>
#include <Wt/WFileResource>

#include <Wt/WLineEdit>
#include <Wt/WTextArea>
#include <Wt/WLabel>
#include <Wt/WText>
#include <Wt/WAnchor>
#include <Wt/WTemplate>
#include <Wt/WComboBox>
#include <Wt/WFileUpload>
#include <Wt/WPushButton>
#include <Wt/WDatePicker>
#include <Wt/WTable>
#include <Wt/WMenu>
#include <Wt/WMessageBox>

#include <Wt/WCssDecorationStyle>


using namespace Wt;
using namespace htmlcxx;

Messages::Messages(dbo::Session& session,WContainerWidget *parent):WContainerWidget(parent),currentType(0),m_messageBodydiv(0),session_(session)
{
    m_mainDiv = new WContainerWidget(this);
    setStyleClass("bodycontent clear_inside");
    m_mainDiv->setStyleClass("faq_box");
    setObjectName("messages");
    update();
}

Messages::~Messages()
{
    //dtor
}

void Messages::footer(WContainerWidget *parent)
{
    WContainerWidget *update_div = new WContainerWidget(parent);
    update_div->setObjectName("update_info");
    update_div->setStyleClass("update_info clear_inside");
    WContainerWidget *wc = new WContainerWidget(update_div);
    wc->setStyleClass("update");
    WContainerWidget *wcp = new WContainerWidget(update_div);

    WAnchor *update_kick = new WAnchor("javascript:void(0)", tr("update"),wc);
    //update_kick->setLink(WLink(WLink::InternalPath,"/private/messages/"));
    update_kick->show();
    update_kick->setObjectName("update_report");
    update_kick->setStyleClass("debt");
    update_kick->clicked().connect(boost::bind(&Messages::changed, this, update_kick));
    new WText(tr("reports.updatebtn.footer"),wcp);

}

void Messages::messageType(WContainerWidget *parent)
{
    WContainerWidget *div = new WContainerWidget(parent);
    div->setStyleClass("tabbable");
    WContainerWidget *messageTypeDiv = new WContainerWidget(div);
    messageTypeDiv->setStyleClass("tabbable");
    WAnchor *aInputMsg = new WAnchor("javascript:void(0)", tr("form.messages.anchor.input"),messageTypeDiv);///private/messages/input
    //aInputMsg->setLink(WLink(WLink::InternalPath,"/private/messages/"));
    WAnchor *aOutputMsg = new WAnchor("javascript:void(0)", tr("form.messages.anchor.output"),messageTypeDiv);///private/messages/output
    //aOutputMsg->setLink(WLink(WLink::InternalPath,"/private/messages/"));
    aOutputMsg->setObjectName("output");
    aInputMsg->setObjectName("input");
    aInputMsg->clicked().connect(boost::bind(&Messages::changed, this, aInputMsg));
    aOutputMsg->clicked().connect(boost::bind(&Messages::changed, this, aOutputMsg));

    new WText(tr("form.messages.label.message.1"),div);
    if(currentType)
        aOutputMsg->setStyleClass("active");
    else
        aInputMsg->setStyleClass("active");

}
/*
void Messages::messageType(WContainerWidget *parent)
{
    WContainerWidget *div = new WContainerWidget(parent);
    div->setStyleClass("tabbable");

    WMenu *tabMenu = new WMenu(div);
    WContainerWidget *messageTypeDiv = new WContainerWidget(div);
    messageTypeDiv->setStyleClass("tab-content");
    WContainerWidget *tabPanel1 = new WContainerWidget(messageTypeDiv);
    tabPanel1->setStyleClass("tab-pane active");
    tabPanel1->setId("tab1");
    WContainerWidget *tabPanel2 = new WContainerWidget(messageTypeDiv);
    tabPanel2->setStyleClass("tab-pane");
    tabPanel2->setId("tab2");

    tabMenu->setStyleClass("nav-tabs");
    tabMenu->addItem(tr("form.messages.anchor.input"),tabPanel1);
    tabMenu->addItem(tr("form.messages.anchor.output"),tabPanel2);
    messageTable(tabPanel1);
    messageTable(tabPanel2);
}*/

void Messages::messageTable(WContainerWidget *parent,WStandardItemModel *model)
{
    //log("info")<<"messageTable(WContainerWidget *paren["<<parent<<"])\n";
    WTable *msgTable = loadMessageList(currentType);//new WTable(parent);
    msgTable->setStyleClass("table table-bordered table-striped");
    msgTable->setHeaderCount(1);
    msgTable->elementAt(0,0)->addWidget(new WText(tr("form.messages.header.coll1")));
    msgTable->elementAt(0,1)->addWidget(new WText(tr("form.messages.header.coll2")));
    msgTable->elementAt(0,2)->addWidget(new WText(tr("form.messages.header.coll3")));
    parent->addWidget(msgTable);
    if(msgTable->rowCount()==1)
    {
        new WText(tr("form.messages.nomessage"),parent);
    }
}

void Messages::messageForm(WContainerWidget *parent)
{

    //log("info")<<"messageForm(WContainerWidget *paren["<<parent<<"])\n";
    ///****************************************************************************************************
    /// Форма ссобщения
    ///****************************************************************************************************
    new WText(tr("form.messages.label.header"),parent);
    WContainerWidget *divSendTo = new WContainerWidget(parent);
    m_cmbSendTo = new WComboBox();
    m_cmbSendTo->addItem(tr("form.messages.cmb.value.1"));
    m_cmbSendTo->addItem(tr("form.messages.cmb.value.2"));
    m_cmbSendTo->addItem(tr("form.messages.cmb.value.3"));
    (new WLabel(tr("form.messages.label.sendto"), divSendTo))->setBuddy(m_cmbSendTo);
    new WText("<br/>",divSendTo);
    //divSendTo->setStyleClass("span2");
    divSendTo->addWidget(m_cmbSendTo);
    new WText("<br/><br/>",parent);

    m_leTheme = new WLineEdit();
    m_leTheme->setStyleClass("span5");
    m_leTheme->setWidth(229);
    m_leTheme->setHeight(19);
    (new WLabel(tr("form.messages.label.theme"), parent))->setBuddy(m_leTheme);
    new WText("<br/>",parent);
    parent->addWidget(m_leTheme);
    new WText("<br/><br/>",parent);

    m_taMessage = new WTextArea();
    m_taMessage->setStyleClass("span5");
    m_taMessage->setColumns(50);
    m_taMessage->setRows(7);
    //message->setWidth(229);
    (new WLabel(tr("form.messages.label.message"), parent))->setBuddy(m_taMessage);
    new WText("<br/>",parent);
    parent->addWidget(m_taMessage);
    new WText("<br/><br/>",parent);

}

void Messages::periodSelector(WContainerWidget *parent)
{
    ///****************************************************************************************************
    /// Выбор периода
    ///****************************************************************************************************
    WLabel *label;
    WContainerWidget *dateSelectForm = new WContainerWidget(parent);
    dateSelectForm ->setStyleClass("filter_options clear_inside");
    dateSelectForm ->setObjectName("dateselectform");


    ///  Рисуем кнопку дата начала
    label = new WLabel(tr("form.messages.begindate"), dateSelectForm );
    m_pBeginDateEdit = new WDatePicker(dateSelectForm );
    label->setBuddy(m_pBeginDateEdit->lineEdit());
    m_pBeginDateEdit->setFormat("dd.MM.yyyy");
    m_pBeginDateEdit->setDate(WDate(WDate::currentDate().year(), WDate::currentDate().month(), 1));
    m_pBeginDateEdit->setTop(WDate::currentDate());
    m_pBeginDateEdit->lineEdit()->validator()->setMandatory(true);


    ///  Рисуем кнопку дата окончания
    label = new WLabel(tr("form.messages.enddate"), dateSelectForm );
    m_pEndDateEdit = new WDatePicker(dateSelectForm );
    label->setBuddy(m_pEndDateEdit->lineEdit());
    //m_pEndDateEdit->setBottom(WDate::currentDate());
    m_pEndDateEdit->setFormat("dd.MM.yyyy");
    m_pEndDateEdit->setDate(WDate::currentDate());
    m_pEndDateEdit->setTop(WDate::currentDate());
    m_pEndDateEdit->lineEdit()->validator()->setMandatory(true);

    ///  Рисуем кнопку применить фильтр
    //WContainerWidget *corner_kick = new WContainerWidget(dateSelectForm );
    //corner_kick->setStyleClass("corner_kick get_filter");
    //new WText("<ins class=\"i1\"></ins><ins class=\"i2\"></ins><ins class=\"i3\"></ins><ins class=\"i4\"></ins>",corner_kick);
    //LineEdit *submitBtn = new LineEdit(corner_kick,LineEdit::Submit);
    WPushButton *submitBtn = new WPushButton(dateSelectForm);
    submitBtn->setValueText(tr("reports.filtered"));
    submitBtn->setObjectName("filter_button");
    submitBtn->clicked().connect(boost::bind(&Messages::changed, this, submitBtn));



}

void Messages::sendMessageForm(WContainerWidget *parent)
{

    //log("info")<<"sendMessageForm(WContainerWidget *paren[)"<<parent<<"]\n";
    messageForm(parent);

    ///****************************************************************************************************
    /// Загрузка файла
    ///****************************************************************************************************
    m_upload = new Wt::WFileUpload();
    m_upload->setFileTextSize(40);
    m_upload->decorationStyle().font().setSize(WFont::Smaller);
    m_upload->setVerticalAlignment(AlignMiddle);
    m_upload->setObjectName("upload_button");

    ///  Рисуем кнопку выбора файла
    WPushButton *uploadButton = new WPushButton(tr("import.order.button.select.file"));
    uploadButton->clicked().connect(m_upload, &WFileUpload::upload);
    uploadButton->clicked().connect(uploadButton, &WPushButton::disable);

    // Upload automatically when the user entered a file.
    m_upload->changed().connect(m_upload, &WFileUpload::upload);
    m_upload->changed().connect(uploadButton, &WPushButton::disable);

    // React to a succesfull upload.
    m_upload->uploaded().connect(this, &Messages::uploaded);

    // React to a fileupload problem.
    m_upload->fileTooLarge().connect(this, &Messages::fileTooLarge);

    (new WLabel(tr("form.messages.label.attachment"), parent));
    new WText("<br/>",parent);
    parent->addWidget(m_upload);
    new WText("<br/><br/>",parent);

    ///  Рисуем кнопку отправить
    /*
    WContainerWidget *captcha = new WContainerWidget(parent);
    captcha->setStyleClass("captcha_form");
    WContainerWidget *divCorner = new WContainerWidget(captcha);
    divCorner->setStyleClass("corner_kick submit");
    *//*
    new WTemplate("<ins class=\"i1\"></ins><ins class=\"i2\"></ins><ins class=\"i3\"></ins><ins class=\"i4\"></ins>",divCorner);
    //WPushButton *submitBtn = new WPushButton(tr("form.messages.btn.send"),div);
    LineEdit *submitBtn = new LineEdit(divCorner,LineEdit::Submit);
    submitBtn->setValueText(tr("form.messages.btn.send"));
    submitBtn->setObjectName("send_button");
    submitBtn->clicked().connect(boost::bind(&Messages::changed, this, submitBtn));
    */
    /*
    WTemplate *button = new WTemplate("<ins class=\"i1\"></ins><ins class=\"i2\"></ins><ins class=\"i3\"></ins><ins class=\"i4\"></ins>${anchor}",divCorner);
    WAnchor *anchor = new WAnchor("#",tr("form.messages.btn.send"));
    anchor->setObjectName("send_button");
    anchor->clicked().connect(boost::bind(&Messages::changed, this, anchor));
    button->bindWidget("anchor",anchor);
    button->setStyleClass("corner_kick submit");
    */
    WPushButton *button = new WPushButton(tr("form.messages.btn.send"),parent);
    button->clicked().connect(std::bind([=](){sendMessage();}));


}

void Messages::readMessageForm(WString msg,WContainerWidget *parent)
{
    /*
    new WText(msg,parent);
    new WText("<br/><br/>",parent);
    WContainerWidget *captcha = new WContainerWidget(parent);
    captcha->setStyleClass("captcha_form");
    WContainerWidget *divCorner = new WContainerWidget(captcha);
    divCorner->setStyleClass("corner_kick submit");
    LineEdit *submitBtn = new LineEdit(divCorner,LineEdit::Submit);
    submitBtn->setValueText(tr("form.messages.btn.close"));
    submitBtn->setObjectName("close_button");
    submitBtn->clicked().connect(boost::bind(&Messages::changed, this, submitBtn));
    */
}

void Messages::update()
{

    //log("info")<<"update()\n";
    m_mainDiv->clear();

    m_messageBodydiv = new WContainerWidget(m_mainDiv);
    m_messageBodydiv->setObjectName("message_form");
    //m_messageBodydiv->clear();
    if(currentType)
        sendMessageForm(m_messageBodydiv);
    else
        readMessageForm("",m_messageBodydiv);
    new WText("<br/><br/>",m_mainDiv);

    //periodSelector(m_mainDiv);
    new WText("<br/><br/>",m_mainDiv);
    messageType(m_mainDiv);
    messageTable(m_mainDiv);
    new WText("<br/><br/>",m_mainDiv);
    footer(m_mainDiv);
}

void Messages::sendMessage()
{
    log("info")<<"sendMessage() "<<__LINE__;


    Mail::Message message;
    message.setFrom(Mail::Mailbox("no-reply@fit-zakaz.ru", "fit-zakaz.ru"));
    //m_cmbSendTo
    //dbo::Transaction transaction(session_);
    //dbo::ptr<User> user = ((Portal *)WApplication::instance())->getSession()->user();
    //UserPropertyList userProperty = session_.find<UserProperty>().orderBy("").where("ref = ?").bind(user);

    std::string location = boost::any_cast<std::string>(((Portal *)WApplication::instance())->getSession()->userProperty("Регион"));
    std::string code = boost::any_cast<std::string>(((Portal *)WApplication::instance())->getSession()->userProperty("Код 1С"));
    std::string ownerForm = boost::any_cast<std::string>(((Portal *)WApplication::instance())->getSession()->userProperty("Форма собственности"));
    std::string name = boost::any_cast<std::string>(((Portal *)WApplication::instance())->getSession()->userProperty("Наименование"));

    //std::string location = ((Portal *)WApplication::instance())->getSession()->getUser()->location;
    //std::string code = ((Portal *)WApplication::instance())->getSession()->getUser()->code;
    //std::string name = ((Portal *)WApplication::instance())->getSession()->getUser()->name;

    std::string email;
    std::string userRef;
    WString snd;
    log("info")<<"index "<<m_cmbSendTo->currentIndex();
    switch(m_cmbSendTo->currentIndex())
    {
            case 0:
                snd = WString::fromUTF8("Оператору");
                userRef = boost::any_cast<std::string>(((Portal *)WApplication::instance())->getSession()->userProperty("Оператор"));
                if(userRef.begin() != userRef.end())
                {
                    email = boost::any_cast<std::string>(((Portal *)WApplication::instance())->getSession()->userProperty("eMail",userRef));
                    if(email.begin() != email.end())
                        message.addRecipient(Mail::To, Mail::Mailbox(email, WString::fromUTF8("Оператор")));
                }
                break;
            case 1:
                snd = WString::fromUTF8("Менеджеру");
                userRef = boost::any_cast<std::string>(((Portal *)WApplication::instance())->getSession()->userProperty("Менеджер"));
                if(userRef.begin() != userRef.end())
                {
                    email = boost::any_cast<std::string>(((Portal *)WApplication::instance())->getSession()->userProperty("eMail",userRef));
                    if(email.begin() != email.end())
                        message.addRecipient(Mail::To, Mail::Mailbox(email, WString::fromUTF8("Менеджер")));
                }
                break;
            case 2:
                snd = WString::fromUTF8("Администратору");
                message.addRecipient(Mail::To, Mail::Mailbox("luban@infotex.org", "Administrator"));
                break;

    }
    log("info")<<"send to "<<snd<<"  "<<userRef<<"  "<<email;
    message.addRecipient(Mail::Bcc, Mail::Mailbox("ineron@me.com", "Administrator"));
    //message.addRecipient(Mail::Bcc, Mail::Mailbox("luban@infotex.org", "Administrator"));
    message.setSubject(WString::fromUTF8("["+location+" "+code+"] ")+m_leTheme->text());
    message.setBody(m_taMessage->text());

    message.addHtmlBody ("<p>"+m_taMessage->text()+"</p><br/><p><strong>"+WString::fromUTF8(ownerForm)+" "+WString::fromUTF8(name)+"</strong></p>");
    std::string jailPath = "/usr/local/jails/jail_webportal";
    std::vector<Http::UploadedFile> files = m_upload->uploadedFiles();
    std::streampos fsize = 0;
    for (unsigned i = 0; i < files.size(); ++i)
    {
        log("info")<<files[i].contentType();
        log("info")<<files[i].spoolFileName();
        std::ifstream file(files[i].spoolFileName().c_str(),std::ios::in | std::ios::binary);
        fsize = file.tellg();
        file.seekg( 0, std::ios::end );
        fsize = file.tellg() - fsize;
        log("info")<<fsize;
        message.addAttachment(files[i].contentType(),files[i].clientFileName(),&file);
        file.close();
    }


    Mail::Client client;
    client.connect("192.168.10.53");
    //client.connect("127.0.0.1");
    if(client.send(message))
        WMessageBox::show(WString::fromUTF8("Уведомление"), WString::fromUTF8("Ваше сообщение отправлено ")+snd,Ok);
    else
        WMessageBox::show(WString::fromUTF8("Уведомление"), WString::fromUTF8("Не удалось отправить сообщение ")+snd,Ok);
    m_taMessage->setText("");
    m_leTheme->setText("");


}

WTable *Messages::loadMessageList(int msgType)
{
    std::string location = boost::any_cast<std::string>(((Portal *)WApplication::instance())->getSession()->userProperty("Регион"));
    std::string code = boost::any_cast<std::string>(((Portal *)WApplication::instance())->getSession()->userProperty("Код 1С"));


    std::string path = WApplication::instance()->appRoot()+"msg/"+location+"/"+code;
    ((Portal *)WApplication::instance())->getSession()->loadMessageList();
    std::vector<db::Message> messageList = ((Portal *)WApplication::instance())->getSession()->getMessageList(msgType);
    std::string r, html;

    WTable *table = new WTable();


    //log("info")<<"***** MESSAGE TYPE: "<<msgType;
    //log("info") <<" PATH *** "<<path+"/list.htm"<<" File-> "<<__FILE__<<" Function-> "<< __FUNCTION__<<" Line ->"<< __LINE__;

    std::fstream ifile(std::string(path+"/list.htm").c_str(), std::ios_base::in);
//    std::fstream ifile("/list.htm", std::ios_base::in);
    if (!ifile)
    {
        //log("info")<< "File "<<path+"/list.htm"<<" no opened";
        return table;
    }

    //string html = "<html><body>hey</body></html>";
    // Читаем в строку
    CharsetConverter cc("WINDOWS-1251","UTF-8");
    while (getline(ifile, r))
        html.append(r);

    ifile.close();
    boost::replace_all( html, "\r", "" );
    html = cc.convert(html);


    HTML::ParserDom parser;
    tree<HTML::Node> dom = parser.parseTree(html);

    //Print whole DOM tree
    //std::cout << dom << std::endl;

    //Dump all links in the tree
    //tree<HTML::Node>::iterator it = dom.begin();
    //tree<HTML::Node>::iterator end = dom.end();
    //int row=0,col=0;
    std::string str="";
    std::string filelink="";
    //log("info")<<"*** MESSAGE COUNT: "<<messageList.size();
    //if(!messageList.size())
    //{
    //    return table;
    //}

    int row_count=0;
    std::vector< db::Message >::iterator i;
    for(i = messageList.begin(); i!= messageList.end() ; ++i)
    {
        db::Message msg = *i;
    //for (unsigned row_count=0; row_count < messageList.size(); row_count++)
    //{
    //    db::Message msg = messageList[row_count];
        //std::cout<<"*** MESSAGE: "<<msg.message;
        //std::cout<<"*** DATE: "<<msg.messageDate.toString("dd.MM.yyyy hh:mm");
        /// Field 1
        //WDateTime fdate(WDateTime::fromTime_t(msg.messageDate));
        //std::cout << "Created       : " << fdate.toString("dd.MM.yyyy hh:mm");
        WString filedate = WString::tr("portal.messages.filedate").arg(msg.messageDate.toString("dd.MM.yyyy hh:mm"));
        table->elementAt(++row_count,0)->addWidget(new WText(filedate));
        /// Field 2
        WContainerWidget *div = new WContainerWidget();
        div->setStyleClass("in");
        str = msg.message;
        div->setToolTip(WString(str,UTF8));
        table->elementAt(row_count,1)->addWidget(div);
        WAnchor *msgAnch;
        if(str.size()>24)
            msgAnch = new WAnchor("#",WString(str,UTF8).value().substr(0,12)+WString("..."),div);
        else
            msgAnch = new WAnchor("#",WString(str,UTF8).value(),div);
        msgAnch->clicked().connect(boost::bind(&Messages::changed, this, msgAnch));
        /// Field 3
        div = new WContainerWidget();
        div->setStyleClass("in");
        table->elementAt(row_count,2)->addWidget(div);
        WFileResource *xFile = new WFileResource("application/vnd.ms-excel", path+"/"+WString::fromUTF8(msg.messageUid).toUTF8());
        xFile->suggestFileName(WString::fromUTF8(msg.fileName));
        (new WAnchor(xFile,WString::fromUTF8(msg.fileName), div));

    }
/*

    for (; it != end; ++it)
    {


        if(it->tagName() == "TR")
        {
            row++;
            col = 0;
            str="";
            filelink = "";
        };
        if(it->tagName() == "TD")col++;

        if ((!it->isTag()) && (!it->isComment()) && (it->text() != "&nbsp;"))
        {
            std::cout << it->text()<<" col "<<col;
            switch (col)
            {
            case 1:
            {
                struct stat64 fileInfo;

                std::string filename = std::string(path+"/"+it->text());//WApplication::instance()->docRoot()+

                std::string date = it->text().substr(14,8);
                date = date.substr(6,2)+"."+date.substr(4,2)+"."+date.substr(0,4);
                std::fstream msgfile(filename, std::ios_base::in);

                int fd =  open(filename.c_str(), O_RDONLY);
                if(!fd)
                {
                    std::cerr << "Error: " << strerror(errno) << " File: "<<__FILE__<<" Line: "<<__LINE__ ;
                }
                if (fstat64(fd, &fileInfo) != 0)    // Use stat( ) to get the info
                {

                    std::cerr << "Error: " << strerror(errno) << " File: "<<__FILE__<<" Line: "<<__LINE__ ;
                    std::cerr << "Error: " <<" Path "<<filename;
                    return  table;//(EXIT_FAILURE);
                }
                close(fd);
                WDateTime fdate(WDateTime::fromTime_t(fileInfo.st_mtime));
                //std::cout << "Created       : " << fdate.toString("dd.MM.yyyy hh:mm");
                WString filedate = WString::tr("portal.messages.filedate").arg(fdate.toString("dd.MM.yyyy hh:mm"));
                table->elementAt(row,col-1)->addWidget(new WText(filedate));

                while (getline(msgfile, r))
                    str.append(r);
                str = cc.convert(str);
                break;
            }
            case 2:
            {
                WContainerWidget *div = new WContainerWidget();
                div->setStyleClass("in");
                div->setToolTip(WString(str,UTF8));
                table->elementAt(row,col-1)->addWidget(div);
                WAnchor *msgAnch;m_messageBodydiv
                if(str.size()>24)
                    msgAnch = new WAnchor("#",WString(str,UTF8).value().substr(0,12)+WString("..."),div);
                else
                    msgAnch = new WAnchor("#",WString(str,UTF8).value(),div);
                msgAnch->clicked().connect(boost::bind(&Messages::changed, this, msgAnch));
                filelink = it->text();
                break;
            }
            case 3:
            {
                WContainerWidget *div = new WContainerWidget();
                div->setStyleClass("in");
                table->elementAt(row,col-1)->addWidget(div);
                //new WAnchor(WLink(WLink::InternalPath,"/private/messages/"+filelink),WString(it->text(),UTF8),div);
                //((Portal *)WApplication::instance())->getSession()->addMessageRecord();
                //WFileResource *fileResource = new Wt::WFileResource("application/pdf", newFileName);
                //fileResource->setDispositionType(Wt::WResource::Attachment);
                //fileResource->suggestFileName(newFileName);

                WFileResource *xFile = new WFileResource("text/xls", path+"/"+filelink);
                xFile->suggestFileName(WString(it->text(),UTF8));
                //xFile->setFileName(filelink);
                (new WAnchor(xFile, WString(it->text(),UTF8),div));//->setRef("/private/messages/"+it->text());
                break;
            }
            }

        }
    }
    */
    return table;
}



void Messages::changed(WWidget* sender)
{

    //log("info")<<"** Element ID "<<sender->objectName();
    /// отправить сообщение
    WAnchor *isAnchor = dynamic_cast<WAnchor*>(sender);

//log("info")<<__LINE__;
    if(isAnchor)
    {
        if(sender->objectName() == "input"){
            currentType = 0;
            update();
            /*
            sender->setStyleClass("active");
            WWidget *w = m_mainDiv->find("output");
            if(w)
            {

                w->removeStyleClass("active");
                m_messageBodydiv->clear();
                //readMessageForm("",m_messageBodydiv);
                //m_messageTable->clear();
                //messageTable(m_mainDiv);
                upadate();

            }
            */
        }else if(sender->objectName() == "output"){
            currentType = 1;
            update();
            /*
            sender->setStyleClass("active");
            WWidget *w = m_mainDiv->find("input");
            if(w)
            {
                w->removeStyleClass("active");
                m_messageBodydiv->clear();
                //sendMessageForm(m_messageBodydiv);
                //m_messageTable->clear();
                //messageTable(m_mainDiv);
                upadate();
            }
            */
        }else if(sender->objectName() == "send_button") {
            sendMessage();
        }else{
            m_messageBodydiv->clear();
            std::string str = sender->parent()->toolTip().toUTF8();
            boost::replace_all( str, "\r", "<br/>" );
            readMessageForm(WString(str,UTF8),m_messageBodydiv);
        };
    };
    //std::cout<<"6\n";
}

void Messages::uploaded()
{
    std::vector<Http::UploadedFile> files = m_upload->uploadedFiles();

    if (!files.empty())
    {
        /*
         * Delete this widgets since we have a succesfull upload.
         */
        //delete m_upload;
        //m_upload = 0;
        //delete remove_;
        //remove_ = 0;
        //delete m_error;
        //m_error = 0;

        //for (unsigned i = 0; i < files.size(); ++i)
        //  m_uploadInfo.push_back(new UploadInfo(files[i], this));
    }
    else
    {
//    m_error->setText(tr("msg.file-empty"));
        m_uploadFailed = true;
    }

    /*
     * Signal to the Composer that a new asynchronous file upload was processed.
     */
    m_uploadDone.emit();
}

void Messages::fileTooLarge(::int64_t size)
{

    WMessageBox::show("Error", WString(tr("msg.file-too-large"))
                      .arg(size / 1024)
                      .arg(WApplication::instance()->maximumRequestSize() / 1024), Ok);
    m_uploadFailed = true;

    /*
     * Signal to the Composer that a new asyncrhonous file upload was processed.
     */
    m_uploadDone.emit();
}
