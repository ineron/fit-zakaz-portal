/*
 * Copyright (C) 2011 Emweb bvba, Kessel-Lo, Belgium.
 *
 * See the LICENSE file for terms of use.
 */
#include "PortalLoginWidget.h"
#include "Session.h"
#include "utils/LineEdit.h"
//#include "../model/Token.h"
#include "User.h"


#include <Wt/WApplication>
#include <Wt/WEnvironment>

#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WTemplate>
#include <Wt/WText>
#include <Wt/WLabel>
#include <Wt/WCheckBox>
#include <Wt/WAnchor>
#include <Wt/WImage>
#include <Wt/Auth/PasswordService>
#include <Wt/Auth/AuthModel>
#include <Wt/Auth/RegistrationWidget>

using namespace Wt;

PortalLoginWidget::PortalLoginWidget(Session& session,
                                     const std::string& basePath,
                                     WContainerWidget *parent)
    : AuthWidget(session.login(), parent),session_(session)
{
    log("info")<<__LINE__;
    setInline(true);

    setObjectName("PortalLoginWidget");
    Auth::AuthModel *model
    = new Auth::AuthModel(session.passwordAuth().baseAuth(),
                          session.users(), this);
    model->addPasswordAuth(&session.passwordAuth());
//    model->addOAuth(session.oAuth());

    setModel(model);

    setInternalBasePath(basePath + "login");

/*
    WApplication *app = WApplication::instance();
    const WEnvironment &env = app->environment();
    const std::string *isTerminal = env.getCookieValue("terminal");
    if(isTerminal)
    {
        if(*isTerminal != std::string(""))
            m_isTerminalMode = true;

        log("info")<<"license "<< *isTerminal<<" "<<__LINE__;
    }
    */

    log("info")<<__LINE__;
}


WFormWidget *PortalLoginWidget::createFormWidget(WFormModel::Field field)
{
    log("info")<<__LINE__;

    WFormWidget *result = 0;
    //WFormWidget *result = Auth::AuthWidget::createFormWidget(field);

    if (field == Auth::AuthModel::LoginNameField)
    {
        WLineEdit *p = new WLineEdit();
        p->setFocus();
        p->setAutoComplete(true);
        //p->setId("login");
        result = p;
    }
    else if (field == Auth::AuthModel::PasswordField)
    {
        WLineEdit *p = new WLineEdit();
        //p->setId("passwd");
        //p->setAutoComplete(false);
        //result->setId("passwd");
        p->enterPressed().connect(this, &AuthWidget::attemptPasswordLogin);
        p->setEchoMode(WLineEdit::Password);

        result = p;
    }
    else if (field == Auth::AuthModel::RememberMeField)
    {
        result = new WCheckBox();
    }
    else if (field == (WFormModel::Field)"terminal")
    {
        result = new WCheckBox();
    }

    log("info")<<__LINE__;
    return result;
}


void PortalLoginWidget::createLoginView()
{
    log("info")<<__LINE__;
    AuthWidget::createLoginView();

    setTemplateText(tr("Wt.Auth.template.login"));

    WLineEdit *userName = resolve<WLineEdit *>("user-name");
    userName->setEmptyText("login");
    userName->setToolTip("login");


    WLineEdit *password = resolve<WLineEdit *>("password");
    password->setEmptyText("password");
    password->setToolTip("password");
    password->enterPressed().connect(this, &PortalLoginWidget::attemptPasswordLogin);


    //WPushButton *login = resolve<WPushButton *>("login");
    //removeWidget(login);
    //LineEdit* login = new LineEdit(0,LineEdit::Submit);
    //login->enterPressed().connect(this, &PortalLoginWidget::attemptPasswordLogin);
    //login->setValueText(tr("Wt.Auth.login"));
    //bindWidget("login",login);

    //password->setObjectName("passwd");
    //userName->setObjectName("login");
    //log("info")<<"pwd "<<password->id()<<" user "<<userName->id();

    ///WCheckBox *remember = resolve<WCheckBox *>("remember-me");
    ///remember->setCheckState(Checked);
    //remember->label()->hide();
    //remember->hide();
    //WWidget *remeberInfo = resolve<WWidget *>("remember-me-info");
    //remeberInfo->hide();
    setCondition("if:terminal",false);
    /*
    m_terminal = new WCheckBox();
    if(m_isTerminalMode)
    {
        m_terminal->setCheckState(Checked);
        //setInternalBasePath("shop");
    }
    m_terminal->changed().connect(this,&PortalLoginWidget::onChangeState);
    bindWidget("terminal",m_terminal);
    */
    log("info")<<__LINE__;
}

void PortalLoginWidget::createLoggedInView()
{
    log("info")<<__LINE__;
    AuthWidget::createLoggedInView();

    //WText *logout = new WText(tr("logout"));
    //logout->setStyleClass("link");
    //logout->clicked().connect(&login(), &Auth::Login::logout);
    //bindWidget("logout", logout);
    bindEmpty("logout");

    //m_wHeader->setId("header");
    //m_wHeader->setStyleClass("header");

    WContainerWidget *languagesDiv = new WContainerWidget();
    languagesDiv->setId("top_languages");
    languagesDiv->hide();
/*
    for (unsigned i = 0; i < languages.size(); ++i)
    {
        if (i != 0)
            new WText("- ", languagesDiv);

        const Lang& l = languages[i];

        new WAnchor(WLink(WLink::InternalPath, l.path_),
                    WString::fromUTF8(l.longDescription_), languagesDiv);
    }
*/
    WContainerWidget *logosDiv = new WContainerWidget();

    logosDiv->setStyleClass("logos");
    //WString kfPage = WString("/shop/{1}").arg(session_.getCatalogFirstPage(2));
    //WAnchor *a = new WAnchor(kfPage.toUTF8(),"", logosDiv);
    //a->setImage(new WImage("/images/logo_1.png"));
    //a->setLink(WLink(WLink::InternalPath,kfPage.toUTF8()));
    //new WImage("/images/logo_2.jpg",logosDiv);
    //a = new WAnchor(WLink(WLink::InternalPath, "/shop/1"),logosDiv);
    //a->setImage(new WImage("/images/logo_3.jpg"));
    //new WImage("/images/logo_4.jpg",logosDiv);
    new WImage("/images/logos.png",logosDiv);

    WAnchor *orderJournal = new WAnchor(WLink(WLink::InternalPath,"/orders/orders_journal"),tr("portal.orders.journal"));
    orderJournal->setObjectName("orderJournalAnc");
    WAnchor *messages = new WAnchor(WLink(WLink::InternalPath,"/private/messages"),tr("portal.messages"));

    bindWidget("header_logos", logosDiv);
    //WString
    //organization = WString::fromUTF8(boost::any_cast<std::string>(session_.userProperty("Форма собственности")));
    //organization += " ";
    //organization += WString::fromUTF8(boost::any_cast<std::string>(session_.userProperty("Наименование")));

    //bindWidget("username",new WLabel(organization));
    bindEmpty("username");
    //bindWidget("languages", languagesDiv);
    bindEmpty("languages");
    bindWidget("current_order",orderJournal);
    bindWidget("messages",messages);
    bindEmpty("current_order");
    bindEmpty("messages");



    log("info")<<__LINE__;
}


void PortalLoginWidget::onChangeState()
{
    log("info")<<__LINE__;
    //m_isTerminalMode = m_terminal->checkState() == Checked;
    //WApplication *app = WApplication::instance();
    //if(!m_isTerminalMode)
    //    app->setCookie("terminal","0",1);
}

