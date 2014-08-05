/*
 * Copyright (C) 2008 Emweb bvba, Kessel-Lo, Belgium.
 *
 * See the LICENSE file for terms of use.
 wsdl2h -o wsclient.h InfoBase.wsdl
 soapcpp2 -i  -a -dgSoap -I/usr/local/share/gsoap/import  wsclient.h
 --docroot ./ --approot ./ --http-addres 127.0.0.1 --http
 */



#include "version.h"
#include "Portal.h"
#include "utils/CsvUtil.h"
#include "db/MsSql7.h"
#include "DeferredWidget.h"
#include "UserProfile.h"
#ifdef GSOAP
#include "soapClient.h"
#endif

#include "Reports.h"
#include "DocumentDetails.h"
#include "Messages.h"

#include "utils/LineEdit.h"
#include "ImportOrder.h"
//#include "PortalLoginWidget.h"
#include "NavigationPanel.h"
#ifndef FIT
#include "CatalogViewWide.h"
#else
#include "CatalogView.h"
#endif
#include "SearchPanel.h"
#include "FoldersPanel.h"
#include "PriceEditor.h"
#include "MainMenu.h"


#include <fstream>
#include <iostream>

#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

#include <Wt/WTimer>
#include <Wt/WContainerWidget>
#include <Wt/WAnchor>
#include <Wt/WLink>
#include <Wt/WApplication>
#include <Wt/WEnvironment>
#include <Wt/WLogger>
#include <Wt/WPushButton>
#include <Wt/WStackedWidget>
#include <Wt/WTemplate>
#include <Wt/WText>
#include <Wt/WDateTime>
#include <Wt/WViewWidget>
#include <Wt/WVBoxLayout>

#include <Wt/WMenu>
#include <Wt/WSubMenuItem>
#include <Wt/WNavigationBar>
#include <Wt/WPopupMenu>
#include <Wt/WPopupMenuItem>


#include <Wt/WImage>
#include <Wt/WComboBox>
#include <Wt/WLineEdit>
#include <Wt/WDialog>
#include <Wt/WPopupWidget>
#include <Wt/WLabel>
#include <Wt/WDatePicker>

#include <Wt/WAbstractItemModel>
#include <Wt/WStandardItemModel>
#include <Wt/WStandardItem>
#include <Wt/WLogger>

#include <Wt/WTable>
//#include <Wt/WTableView>
#include <Wt/WTabWidget>
#include <Wt/WTableCell>

#include <Wt/WLength>

#include <Wt/Auth/AuthWidget>

#include <web/DomElement.h>

#include <Wt/WMessageBox>
#include <Wt/Auth/PasswordPromptDialog>

#include <Wt/WBootstrapTheme>

#include <Wt/WLengthValidator>
//#include "view/BlogView.h"

using namespace Wt;

static const std::string SHOP_INTERNAL_PATH = "shop";

Portal::~Portal()
{
    //delete m_domRootFooter;
#ifdef GSOAP
    delete m_soapClient;
#endif


}

Portal::Portal(const WEnvironment& env, const std::string& title,
               const std::string& resourceBundle, const std::string& cssPath)
    : WApplication(env),
    m_content(0),
    //m_domRootHeader(0),
    //m_domRootFooter(0),
    m_rightSide(0),
    m_upPanel(0),
    m_downPanel(0),
    m_searchPanel(0),
    m_folderPanel(0),
    m_footer(0),
    //releases_(0),
    m_wHeader(0),
    m_pageHeader(0),
    m_wHomePage(0),
    m_catalogView(0),
    navigation_(0),
    m_isTerminalMode(false),
    m_accessStatus(false),
    m_isCatalogMode(false)
#ifdef GSOAP
    ,m_soapClient(0)
#endif

    //m_currentPage(1)
    //m_lastRecord(0)
{
    messageResourceBundle().use(appRoot() + "xml/templates", false);
    messageResourceBundle().use(appRoot() + "xml/strings", false);
    messageResourceBundle().use(appRoot() + "xml/services", false);
    messageResourceBundle().use(appRoot() + "xml/news", false);
    messageResourceBundle().use(appRoot() + "xml/whatsnew", false);
    messageResourceBundle().use(appRoot() + "xml/soapQueryes", false);



#ifdef DEBUG
    log("info")<<"Agent is "<<environment().agent();
#endif

    useStyleSheet("/css/exbs.css");
    useStyleSheet("/css/menu.css");
    setTitle(title);
    WBootstrapTheme *bootstrapTheme = new WBootstrapTheme(this);
    //Wt::WApplication::instance()->styleSheet().removeRule(new WCssTemplateRule(".dropdown-toggle"));
    //bootstrapTheme->setVersion(Wt::WBootstrapTheme::Version3);
    bootstrapTheme->setResponsive(true);
    setTheme(bootstrapTheme);
    useStyleSheet(WApplication::relativeResourcesUrl() + "form.css");
    language_ = 0;

}

void Portal::loadCSS(WApplication *app)
{

    boost::this_thread::sleep(boost::posix_time::milliseconds(100));

    {
        WApplication::UpdateLock uiLock(app);
        if (uiLock)
        {

            doJavaScript(
                "console.log('stuff-1');"
            );

            //instance()->require("/shared/bootstrap-carousel.js");

            instance()->require("/shared/highslide/highslide.packed.js");
            useStyleSheet("/shared/highslide/highslide.css");


            std::string highslidejs = INLINE_JAVASCRIPT
                                      (
                                          hs.graphicsDir = "/shared/highslide/graphics/";
                                          hs.outlineType = null;
                                          hs.showCredits = false;
                              hs.lang= {cssDirection:"ltr",loadingText:"Загрузка...",loadingTitle:"Кликните чтобы отменить",focusTitle:"Нажмите чтобы перенести вперёд",fullExpandTitle:"Увеличить",fullExpandText:"Полноэкранный",previousText:"Предыдущий",previousTitle:"Назад (стрелка влево)",nextText:"Далее",nextTitle:"Далее (стрелка вправо)",moveTitle:"Передвинуть",moveText:"Передвинуть",closeText:"Закрыть",closeTitle:"Закрыть (Esc)",resizeTitle:"Восстановить размер",playText:"Слайд-шоу",playTitle:"Слайд-шоу (пробел)",pauseText:"Пауза",pauseTitle:"Приостановить слайд-шоу (пробел)",number:"Изображение %1/%2",restoreTitle:"Нажмите чтобы посмотреть картинку, используйте мышь для перетаскивания. Используйте клавиши вперёд и назад"};
                                      );
            doJavaScript(highslidejs);

            //doJavaScript("<script src=""/shared/highslide/highslide.packed.js""></script>");

            //"$('.carousel').carousel();"
            /*doJavaScript(
                "$('.carousel').carousel();"
            );*/

            doJavaScript("flyMenuObj.init()");


            app->triggerUpdate();
        }
    }


    boost::this_thread::sleep(boost::posix_time::milliseconds(200));
    {
        WApplication::UpdateLock uiLock(app);
        if (uiLock)
        {
            for(auto i=folderIdNameMap_.begin(); i!=folderIdNameMap_.end(); ++i)
            {
                if(WWidget *w = i->second)
                {
                    w->removeStyleClass("nav");
                }
            }

            app->triggerUpdate();
        }
    }

    app->enableUpdates(false);


}

void Portal::init()
{
#ifdef DEBUG
    log("info")<<"********* INIT *********";
#endif
    require("http://www.google-analytics.com/ga.js");
    doJavaScript(
        "window.pageTracker = _gat._getTracker('UA-47286488-2');"
        "window.pageTracker._initData();"
        "window.pageTracker._trackPageview();"
    );
    //declareJavaScriptFunction("GoogleAnalysticAsync",GoogleAnalysticAsync);
    //declareJavaScriptFunction("GoogleAnalystic",GoogleAnalystic);

    //setBodyClass("bodycontent");
    if(environment().agent() < WEnvironment::IE8 )
        WMessageBox::show(WString::fromUTF8("Предупреждение !"), WString("Вы используете очень страрую версию браузера, при работе с Вашим браузером могут возникать ошибки в работе системы !<br/>Мы настоятельно рекомендуем утсановить более новую версию браузера !<br/>Вы так же можете загрузить и установить безплатные брайзеры Chrome или Firefox",UTF8), Ok);

    if(environment().agent() >= WEnvironment::Chrome0 && environment().agent() < WEnvironment::Chrome5)
        WMessageBox::show(WString::fromUTF8("Предупреждение !"), WString("Вы используете устаревшую версию браузера, при работе с Вашим браузером могут возникать ошибки в работе системы !<br/>Мы настоятельно рекомендуем утсановить более новую версию браузера !",UTF8), Ok);

    if(environment().agent() >= WEnvironment::Firefox && environment().agent() < WEnvironment::Firefox5_0)
        WMessageBox::show(WString::fromUTF8("Предупреждение !"), WString("Вы используете устаревшую версию браузера, при работе с Вашим браузером могут возникать ошибки в работе системы !<br/>Мы настоятельно рекомендуем утсановить более новую версию браузера !",UTF8), Ok);

    if(environment().agent() >= WEnvironment::Opera && environment().agent() < WEnvironment::Opera10)
        WMessageBox::show(WString::fromUTF8("Предупреждение !"), WString("Вы используете устаревшую версию браузера, при работе с Вашим браузером могут возникать ошибки в работе системы !<br/>Мы настоятельно рекомендуем утсановить более новую версию браузера !",UTF8), Ok);

    internalPathChanged().connect(this, &Portal::setup);
    internalPathChanged().connect(this, &Portal::setLanguageFromPath);
    internalPathChanged().connect(this, &Portal::logInternalPath);
    session_.login().changed().connect(this, &Portal::onAuthEvent);
    Auth::AuthModel *authModel = new Auth::AuthModel(Session::auth(),
            session_.users(), this);
    authModel->addPasswordAuth(&Session::passwordAuth());
    //authModel->addOAuth(Session::oAuth());
    Auth::AuthWidget *authWidget = new Auth::AuthWidget(session_.login());
    //Auth::AuthWidget *authWidget = new PortalLoginWidget(session_,"/");
    authWidget->setModel(authModel);
    authWidget->setRegistrationEnabled(false);
    //m_domRootHeader->addWidget(authWidget);
    root()->addWidget(authWidget);
    //std::cout<<"********** Check state "<<((PortalLoginWidget*)authWidget)->getTerminalMode()<<std::endl;

    //int m_maxPage = session_.maxPage();
    setLanguageFromPath();

    authWidget->processEnvironment();

#ifdef DEBUG
    log("info")<<"********* INIT END *********";
#endif


}

/*
void Portal::handleInternalPath(const std::string &internalPath)
{
    //log("info")<<"*********************************************";
    //log("info") << "Internal path = "<<internalPath;
    if (internalPath == "/order")
    {
        log("info")<<"*********************************************";
        wrapView(&Portal::currentOrder)->show();

    }
}
*/
void Portal::setup()
{
    /*
     * This function switches between the two major components of the homepage,
     * depending on the internal path:
     * /src -> source viewer
     * /... -> homepage
     *
     * FIXME: we should take into account language /cn/src ...
     */
#ifdef DEBUG
    log("info")<<"********** SETUP **************"<<__LINE__;
    log("info")<<"version "<<AutoVersion::_FULLVERSION_STRING<<" status "<<AutoVersion::_STATUS<<" build "<<AutoVersion::_BUILDS_COUNT;
#endif
    std::string base = internalPathNextPart("/");
#ifdef DEBUG
    log("info")<< "BASE PATH "<<base;
#endif

    //WApplication::requireJQuery();
    //setCssTheme("polished");
    //if(base == "shop")
    //    m_isCatalogMode = true;
    //useStyleSheet("/css/styles.css");
    //useStyleSheet("/css/style1.css");

    //useStyleSheet("http://netdna.bootstrapcdn.com/bootstrap/3.0.3/css/bootstrap.min.css");
    //useStyleSheet("http://netdna.bootstrapcdn.com/bootstrap/3.0.3/css/bootstrap-theme.min.css");
    //instance()->requireJQuery("http://code.jquery.com/jquery.min.js");
    //instance()->require("http://netdna.bootstrapcdn.com/bootstrap/3.0.3/js/bootstrap.min.js");

    instance()->require("/css/ie_console.js");

    //std::string base = internalPathNextPart("/");

    if (session_.login().loggedIn())
    {

        int newMode = 0;

        //session_.setCurrentOrder(getLisenseId());

        if(base == "shop")
            newMode = 1;
        else if(base == "information")
            newMode = 2;
        else if(base == "terminal")
            newMode = 2;

        if(!m_wHomePage)
        {
            m_content = new WStackedWidget();
            m_rightSide = new WContainerWidget();
            //m_rightSide->setId("search_panel");
            //m_rightSide->removeStyleClass("block_cover");
            //m_rightSide->setStyleClass("block_cover");
            //createHeader();

            m_wHomePage = createHome();
            //m_wHomePage = new WContainerWidget(root());
            m_isCatalogMode = newMode;
            createRightPanel(newMode);
            m_footer = createFooter();
            root()->addWidget(createFooter());

        }
        else
        {


            if(m_isCatalogMode != newMode)
            {
#ifdef DEBUG
                log("info")<<"*** Change catalog mode "<<newMode;
#endif
                createRightPanel(newMode);
                m_isCatalogMode = newMode;
            }

#ifdef DEBUG
            log("info")<<"*** is Catalog mode "<<m_isCatalogMode;
#endif

            if(m_isCatalogMode==1)
            {
                int currentPage = atoi(internalPathNextPart("/shop/").c_str());
                if (currentPage <= 0)
                    currentPage = 1;
#ifdef DEBUG
                log("info")<<"******** CURRENT PAGE "<<currentPage<<"  "<<__LINE__;
#endif
                //if(currentPage)
                //    setInternalPath((WString("/shop/{1}").arg(currentPage)).toUTF8().c_str());
                m_downPanel->setCurrentPage(currentPage);
                m_upPanel->setCurrentPage(currentPage);
                //m_folderPanel->setCurrentPage(currentPage);
                //m_folderPanel->update();

                m_catalogView->setCurrentPage(currentPage);
                m_catalogView->update();
                if(m_pageHeader)
                {
                    WString str = WString("<H1>{1}</H1>").arg(session_.getPageHeader(currentPage));
                    ((WText*)m_pageHeader)->setText(str);
                }

            }
            //else
            //log("info")<<" base "<<base;
            if(base == std::string("orders"))
            {
                if(WContainerWidget *cont = dynamic_cast<WContainerWidget*>(m_content->currentWidget()))
                {
                    if(WContainerWidget *cont2 = dynamic_cast<WContainerWidget*>(cont->widget(0)))
                    {
                        if(Reports *rpt = dynamic_cast<Reports*>(cont2->widget(0)))
                        {
                            rpt->update();
                        }
                    }
                }
            }
            else if(base == std::string("private"))
            {
                if(WContainerWidget *cont = dynamic_cast<WContainerWidget*>(m_content->currentWidget()))
                {
                    if(WContainerWidget *cont2 = dynamic_cast<WContainerWidget*>(cont->widget(0)))
                    {
                        if(Messages *msg = dynamic_cast<Messages*>(cont2->widget(0)))
                        {
                            log("info")<<" MESSAGES "<<__LINE__;
                            msg->update();
                        }
                    }
                }
            }


            //log("info")<<m_content->currentWidget()->id();
        }

    }

#ifdef DEBUG
    log("info")<<"********** SETUP **************"<<__LINE__;
#endif
    enableUpdates(true);
    workThread_  = boost::thread(boost::bind(&Portal::loadCSS, this, this));



}

void Portal::sendOrders()
{
#ifdef DEBUG
    log("info")<<"******** SEND ORDERS *********"<<__LINE__;
#endif
}

void Portal::onAuthEvent()
{
#ifdef DEBUG
    log("info")<<"******** onAuthEvent *********"<<__LINE__;
#endif
    if (session_.login().loggedIn())
    {

        session_.removeOldToken();
        //WApplication::instance()->doJavaScript("$('#loginForm').submit();");
        //m_domRootHeader->clear();
        //PortalLoginWidget* logwidget = dynamic_cast<PortalLoginWidget*>(root()->widget(0));

        //std::string scr = "(function(){var ac,c,f,fa,fe,fea,x,y,z;ac=\"autocomplete\";c=0;f=document.forms;for(x=0;x<f.length;x++){fa=f[x].attributes;for(y=0;y<fa.length;y++){if(fa[y].name.toLowerCase()==ac){fa[y].value=\"on\";c++;}}fe=f[x].elements;for(y=0;y<fe.length;y++){fea=fe[y].attributes;for(z=0;z<fea.length;z++){if(fea[z].name.toLowerCase()==ac){fea[z].value=\"on\";c++;}}}}alert(\"Enabled'\"+ac+\"'on\"+c+\"objects.\");})();";
        //logwidget->doJavaScript("$('#loginForm').submit();");
        //logwidget->doJavaScript(scr);

        //if(WContainerWidget *logwidget = dynamic_cast<WContainerWidget *>(WApplication::instance()->root()->find("PortalLoginWidget")))
        //{
        //log("info")<<"*********************************************";
        //log("info")<<"logwidget "<<logwidget->objectName();
        //}
        const std::string *isTerminal = environment().getCookieValue("terminal");

        m_isTerminalMode = boost::any_cast<bool>(session_.userProperty("Режим терминала"));
#ifdef DEBUG

        //log("info")<<"m_isTerminalMode "<<m_isTerminalMode;
        log("info")<<"Logged in "<<session_.getUser();
#endif


        if(m_isTerminalMode)
        {
            if(isTerminal)
            {
                if(*isTerminal == "")
                {
                    //m_licenseId = session_.getTerminalLicense();
                    //log("info")<<"license "<<m_licenseId<<" "<<__LINE__;
                    if(m_licenseId != std::string(""))
                    {
                        setCookie("terminal",m_licenseId,31536000);
                        //log("info")<<"license "<<m_licenseId<<" "<<__LINE__;
                    }
                    else
                    {
                        m_isTerminalMode = false;
                        //WMessageBox::show("Notice", WString("Для Вашего кабинета, нет свободных лицензий. </br><strong>Обратитесь в службу поддержки.</strong>",UTF8), Ok);
                    }


                }
                else
                {
                    m_licenseId = *isTerminal;
                    //log("info")<<"license "<<m_licenseId<<" "<<__LINE__;
                }

            }
            else
            {
                //m_licenseId = session_.getTerminalLicense();
                //log("info")<<"license "<<m_licenseId<<" "<<__LINE__;
                if(m_licenseId != std::string(""))
                {
                    //log("info")<<"license "<<m_licenseId<<" "<<__LINE__;
                    setCookie("terminal",m_licenseId,31536000);
                }
            }
        }
        else
        {
            if(isTerminal)
                if(*isTerminal != std::string(""))
                    session_.setFreeTerminalLicense(*isTerminal);
            m_licenseId = "";
        }
        //log("info")<<"license "<<m_licenseId<<" "<<__LINE__;


        //root()->clear();
#ifdef GSOAP
        std::string location = boost::any_cast<std::string>(session_.userProperty("Регион"));
        m_soapClient = new SoapClient(location);
#endif

        setup();
        session_.setCurrentOrder();
        //session_.loadMessageList();

    }
    else
    {
        //root()->clear();
        //log("info")<<"Home "<<m_wHomePage;
        if(m_wHomePage)
            root()->removeWidget(m_wHomePage);
        //((WContainerWidget*)m_wHomePage)->clear();
        m_wHomePage = 0;

    }
#ifdef DEBUG
    log("info")<<"******** onAuthEvent *********"<<__LINE__;
#endif

}


void Portal::logout()
{

}

void Portal::createHeader()
{
    //WTemplate *m_mainTemplate = new WTemplate(tr("template.content.header"), m_domRootHeader);
    WTemplate *mainTemplate = new WTemplate(tr("template.content.header"), root());
    m_wHeader = mainTemplate;
    m_wHeader->setId("header");
    m_wHeader->setStyleClass("header");

    WContainerWidget *languagesDiv = new WContainerWidget();
    languagesDiv->setId("top_languages");
    languagesDiv->hide();
    for (unsigned i = 0; i < languages.size(); ++i)
    {
        if (i != 0)
            new WText("- ", languagesDiv);

        const Lang& l = languages[i];

        new WAnchor(WLink(WLink::InternalPath, l.path_),
                    WString::fromUTF8(l.longDescription_), languagesDiv);
    }

    WContainerWidget *logosDiv = new WContainerWidget();

    logosDiv->setStyleClass("logos");
    //WString kfPage = WString("/shop/{1}").arg(session_.getCatalogFirstPage(2));
    //WAnchor *a = new WAnchor(kfPage.toUTF8(),"", logosDiv);
    //a->setImage(new WImage("/images/logo_1.png"));
    //a->setLink(WLink(WLink::InternalPath,kfPage.toUTF8()));
    //new WImage("/images/logo_2.jpg",logosDiv);
    //a = new WAnchor(WLink(WLink::InternalPath, "/shop/1"),logosDiv);
    //a->setImage(new WImage("/images/logo_3.jpg"));
    new WImage("/images/logos.jpg",logosDiv);
    //new WImage("/images/logo_5.png",logosDiv);

    WAnchor *orderJournal = new WAnchor(WLink(WLink::InternalPath,"/orders/orders_journal"),tr("portal.orders.journal"));
    orderJournal->setObjectName("orderJournalAnc");
    orderJournal->clicked().connect(boost::bind(&Portal::onClick, this, orderJournal));
    WAnchor *messages = new WAnchor(WLink(WLink::InternalPath,"/private/messages"),tr("portal.messages"));

    // Меню выхода
    //WText *logout = new WText(tr("logout"),m_domRootHeader);
    WText *logout = new WText(tr("logout"),root());
    //logout->setStyleClass("exit");
    logout->show();
    logout->clicked().connect(boost::bind(&Portal::logout, this));

    //WWidget *m_wHomePage = createHome();

    mainTemplate->bindWidget("header_logos", logosDiv);
    mainTemplate->bindWidget("languages", languagesDiv);
    mainTemplate->bindWidget("current_order",orderJournal);
    mainTemplate->bindWidget("messages",messages);
    mainTemplate->bindWidget("logout",logout);
    //m_mainTemplate->bindWidget("content",m_wHomePage);

    //result->bindWidget("languages", languagesDiv);

}


WPopupMenu *Portal::makeShopMenu()
{

    //WStandardItemModel *folderModel_ = new WStandardItemModel(0, 1, this);
    WPopupMenu *popup = new Wt::WPopupMenu(m_content);

    //std::map<std::string, WWidget*> folderIdNameMap_;

    session_.getFoldersModel(popup,folderIdNameMap_);

    return popup;

}

void Portal::createMainMenu()
{
#ifdef DEBUG
    log("info")<<"createMainMenu() "<<__LINE__;
#endif

    navigation_ = new WNavigationBar();
    navigation_->addStyleClass("navbar-green");
#ifndef FIT
    navigation_->setTitle(Wt::WString::fromUTF8("БМК-ХОЛОД ЛК"), "/");
#else
    navigation_->setTitle(Wt::WString::fromUTF8("FIT ЛК"), "/");
#endif
    navigation_->setResponsive(true);

    //navigation_->setId("menutop");



    mainMenu_ = new WMenu(m_content);
    navigation_->addMenu(mainMenu_);
    mainMenu_->setInternalPathEnabled();
    mainMenu_->setInternalBasePath( "/" );
    WMenuItem *mi;

    //mainMenu_->setStyleClass("menutop clear_inside");
    //mi = mainMenu_->addItem(tr("news"), wrapView(&Portal::news), WMenuItem::PreLoading);//->setPathComponent("news/");
    //mi = new MainMenuItem(tr("news"), deferCreate(boost::bind(&Portal::news, this)));
    mi = mainMenu_->addItem(tr("news"), news());
    //mi = mainMenu_->addItem(new MainMenuItem(tr("news"), deferCreate(boost::bind(&Portal::news, this))));
    //paretItem[0] = mi;
    mi->setPathComponent("");





    ///***************************************************************************************************************************
    ///   Меню Личный кабинет
    ///***************************************************************************************************************************
    ///WSubMenuItem *subMenuCabinet = new WSubMenuItem(tr("private"),deferCreate(boost::bind(&Portal::privateDate, this)), WMenuItem::PreLoading);
    //WMenu *subMenuCabinetItems = new WMenu(m_content);
    Wt::WPopupMenu *popup = new Wt::WPopupMenu(m_content);
    popup->setInternalPathEnabled("/private/");

    WString userName = WString::fromUTF8(boost::any_cast<std::string>(session_.userProperty("Наименование")));

    mi = popup->addItem(userName, deferCreate(boost::bind(&Portal::userCart, this)));
    mi->setPathComponent("usercard");


    popup->addSeparator();

    mi = popup->addItem(tr("introduction"), deferCreate(boost::bind(&Portal::introduction, this)));
    //mi->itemWidget()->setId("menu_intro");
    mi = popup->addItem(tr("operator"), deferCreate(boost::bind(&Portal::operators, this)));
    //mi->itemWidget()->parent()->setId("menu_operator");
    mi = popup->addItem(tr("messages"), deferCreate(boost::bind(&Portal::messages, this)));

    popup->addSeparator();

    mi = popup->addItem(tr("exit"), &session_.login(), &Auth::Login::logout);
    //mi = mainMenu_->addMenu(tr("private"),subMenuCabinetItems);
    Wt::WMenuItem *item = new Wt::WMenuItem(tr("user"));
    //item->setMenu(popup);
    //mainMenu_->addItem(item);

    item->setMenu(popup);
    Wt::WMenu *rightMenu = new Wt::WMenu();
    rightMenu->addItem(item);

    item->setPathComponent("private");


    ///***************************************************************************************************************************
    ///   Меню Заказы
    ///***************************************************************************************************************************
    //mi = mainMenu_->addItem(tr("operator"),wrapView(&Portal::operators), WMenuItem::PreLoading);
    ///WSubMenuItem *subMenuOrders = new WSubMenuItem(tr("orders"),wrapView(&Portal::orders), WMenuItem::PreLoading);
    //WMenu *subMenuOrdersItems = new WMenu(m_content);
    popup = new Wt::WPopupMenu(m_content);
    popup->setInternalPathEnabled("/orders/");
    //subMenuOrdersItems->setRenderAsList(true);


    mi = popup->addItem(tr("order.details"), deferCreate(boost::bind(&Portal::orderDetails, this)));
    //mi = subMenuOrdersItems->addItem(tr("order.details"), Portal::orderDetails());
    mi->setPathComponent("current_order");
    mi->contents()->setObjectName("reports_current_order");
    mi = popup->addItem(tr("portal.orders.journal"), deferCreate(boost::bind(&Portal::ordersJournal, this)));
    mi->setPathComponent("orders_journal");
    mi = popup->addItem(tr("portal.orders.import"), deferCreate(boost::bind(&Portal::importOrder, this)));
    mi->setPathComponent("import_order");


    // Устанавливаем созданое меню как сабменю
    //mi = mainMenu_->addMenu(tr("orders"),subMenuOrdersItems);
    item = new Wt::WMenuItem(tr("orders"));
    item->setMenu(popup);
    mainMenu_->addItem(item);
    item->setPathComponent("orders");


    ///***************************************************************************************************************************
    ///   Меню отчеты
    ///***************************************************************************************************************************
    //Создаем подчиненое меню
    ///WSubMenuItem *subMenu = new WSubMenuItem(tr("reports"),wrapView(&Portal::reports), WMenuItem::PreLoading);
    //WMenu *subMenuItems = new WMenu(m_content);
    popup = new Wt::WPopupMenu(m_content);
    popup->setInternalPathEnabled("/reports/");
    //subMenuItems->setInternalBasePath("reports/");
    //subMenuItems->setInternalPathEnabled();
    mi = popup->addItem(tr("mutual"), 		deferCreate(boost::bind(&Portal::mutual, 	this)));
    mi = popup->addItem(tr("order_journal"), 	deferCreate(boost::bind(&Portal::order_journal, this)));
    mi = popup->addItem(tr("dept_structure"), 	deferCreate(boost::bind(&Portal::dept_structure,this)));
    mi = popup->addItem(tr("sales"), 		deferCreate(boost::bind(&Portal::sales, 	this)));

    // Устанавливаем созданое меню как сабменю
    //WMenuItem* subItem = new WMenuItem(tr("reports"),reports());
    //subItem->setMenu(subMenuItems);
    //mainMenu_->addItem(subItem);
    //mi = subItem;
    //mi = mainMenu_->addMenu(tr("reports"),subMenuItems);
    item = new Wt::WMenuItem(tr("reports"));
    item->setMenu(popup);
    mainMenu_->addItem(item);

    //mi->setPathComponent("reports");

    ///***************************************************************************************************************************
    ///   Меню Полезная информация
    ///***************************************************************************************************************************
    ///Wt::WSubMenuItem *subMenu2 = new WSubMenuItem(tr("information"),wrapView(&Portal::information), WMenuItem::PreLoading);
    //Wt::WMenu *subMenuItems2 = new WMenu(m_content);
    popup = new Wt::WPopupMenu(m_content);
    popup->setInternalPathEnabled("/information/");
    mi = popup->addItem(tr("services"), wrapView(&Portal::services), WMenuItem::PreLoading);
    mi = popup->addItem(tr("whatsnew"), wrapView(&Portal::whatsnew), WMenuItem::PreLoading);

    //mi = mainMenu_->addMenu(tr("information"),subMenuItems2);
    item = new Wt::WMenuItem(tr("information"));
    item->setMenu(popup);
    mainMenu_->addItem(item);
    item->setPathComponent("information");


    ///***************************************************************************************************************************
    ///   Меню Терминалов
    ///***************************************************************************************************************************
    if(boost::any_cast<bool>(session_.userProperty("Редактор цен покупателя")))
    {
        ///Wt::WSubMenuItem *subMenuTerminal = new WSubMenuItem(tr("terminal"),wrapView(&Portal::terminal), WMenuItem::PreLoading);
        //log("info")<<" Создаем меню редактор цен пользователя";
        //Wt::WMenu *subMenuTerminalItems = new WMenu(m_content);
        popup = new Wt::WPopupMenu(m_content);
        popup->setInternalPathEnabled("/terminal/");
        mi = popup->addItem(tr("priceEditor"), deferCreate(boost::bind(&Portal::priceEditor, this)));
        mi = popup->addItem(tr("loadPrice"), deferCreate(boost::bind(&Portal::loadPrice, this)));

        //mi = subMenuTerminalItems->addItem(tr("setpassword"), this, &Portal::setpassword);
        //mi = mainMenu_->addMenu(tr("terminal"),subMenuTerminalItems);

        item = new Wt::WMenuItem(tr("terminal"));
        item->setMenu(popup);
        mainMenu_->addItem(item);

    }



    popup = makeShopMenu();
    item = new Wt::WMenuItem(tr("shop"),deferCreate(boost::bind(&Portal::createCatalog, this)));

    item->setMenu(popup);

    mainMenu_->addItem(item);
    //item->widget(0)->removeStyleClass("dropdown-toggle",true);

    //mi = mainMenu_->addItem(tr("shop"), deferCreate(boost::bind(&Portal::createCatalog, this)));
    //mi->setPathComponent("shop");

    if(session_.userName() == std::string("admin"))
    {
        //mi = mainMenu_->addItem(tr("admin"), deferCreate(boost::bind(&Portal::adminPanel, this)));
    }

    for(auto i=folderIdNameMap_.begin(); i!=folderIdNameMap_.end(); ++i)
    {
        if(WWidget *w = i->second)
        {
            w->removeStyleClass("nav");
        }
    }


    mainMenu_->itemSelectRendered().connect(this, &Portal::updateTitle);


    Wt::WLineEdit *searchEdit = new Wt::WLineEdit();
    searchEdit->setObjectName("poisk");
    searchEdit->setEmptyText(Wt::WString::fromUTF8("Поиск по сайту"));

    searchEdit->setValidator(new Wt::WLengthValidator(0, 200));
    searchEdit->enterPressed().connect(this,&Portal::onEnterPressed);
    //searchEdit->changed().connect(this,&Portal::onEnterPressed);

    searchEdit->enterPressed().connect(std::bind([=] ()
    {
        //model->setFilter(0,edit->text().toUTF8());
        setInternalPath ("/shop", true);
        onSearch(searchEdit->text());

    }));

    searchEdit->addStyleClass("search-query");
    WContainerWidget *searchDiv = new WContainerWidget();
    WContainerWidget *clearSearch = new WContainerWidget();
    clearSearch->clicked().connect(std::bind([=] ()
    {
        setInternalPath ("/shop", true);
        searchEdit->setText("");
        onSearch("");

    }));

    WContainerWidget *btnSearch = new WContainerWidget();
    btnSearch->setStyleClass("search-btn");
    btnSearch->clicked().connect(std::bind([=] ()
    {
        setInternalPath ("/shop", true);
        onSearch(searchEdit->text());

    }));

    searchDiv->addWidget(btnSearch);
    searchDiv->addWidget(searchEdit);
    searchDiv->addWidget(clearSearch);
    clearSearch->setStyleClass("search-del");
    //navigation_->addSearch(searchEdit);

    //SearchPanel *searchPanel = new SearchPanel(m_rightSide);
    //searchPanel->onSearchChanged().connect(this,&Portal::onSearch);

    if(environment().agent() >= WEnvironment::IE10 )
        navigation_->addWidget(searchDiv);

    navigation_->addMenu(rightMenu, Wt::AlignRight);
#ifdef DEBUG
    log("info")<<"createMainMenu() "<<__LINE__;
#endif
}

WWidget *Portal::createPreviewButton()
{
    WContainerWidget *wc = new WContainerWidget();
    wc->setStyleClass("preview");
    wc->setId("preview");

    WAnchor *btn = new WAnchor(wc);
    btn->setStyleClass("corner_kick");
    btn->setText(tr("search.preview.button"));
    btn->setLink(WLink(WLink::InternalPath,"/orders/current_order"));
    //btn->setLink("javascript:void(0)");
    btn->setTarget(TargetSelf);
    btn->setObjectName("previewbtn");
    //btn->setRefInternalPath("/reports/order_details");
    //WApplication::instance()->internalPathChanged().connect(this, &Portal::setup);
    //btn->clicked().connect(this ,&CatalogMainPage::showOrder);
    //btn->clicked().connect(boost::bind(&Portal::onClick, this, btn));
    btn->show();
    return wc;

}

WWidget *Portal::createRightPanel(int mode)
{
#ifdef DEBUG
    log("info")<<"createRightPanel() "<<__LINE__;
#endif
    m_rightSide->clear();
    //log("info")<<"createRightPanel(-) "<<__LINE__;
    //WAnimation fade(WAnimation::Fade, WAnimation::Linear, 150);
    //m_content->setTransitionAnimation(fade);

    // Создаем основное меню

    if(mode==1)
    {
        //log("info")<<"createRightPanel(1) "<<__LINE__;
        //m_rightSide->setId("search_panel");
        m_rightSide->removeStyleClass("block_cover");
        if(environment().agent() <= WEnvironment::IE10 )
        {
            SearchPanel *searchPanel = new SearchPanel(m_rightSide);
            searchPanel->onSearchChanged().connect(this,&Portal::onSearch);
        }
        m_rightSide->addWidget(createPreviewButton());
        m_folderPanel = new FoldersPanel(m_rightSide);



        m_folderPanel->onPagechanged().connect(this,&Portal::onPageChanged);
    }
    else if(!mode)
    {

        ////log("info")<<"createRightPanel(2) "<<__LINE__;
        ////m_rightSide->setId("block_cover");
        //m_rightSide->setStyleClass("block_cover");
        //WAnchor *catalog_hd_;
        //WAnchor *catalog_px_;

        //WContainerWidget* wcr = new  WContainerWidget(m_rightSide);
        //wcr->setStyleClass("title");
        //catalog_hd_ = new WAnchor("/shop", tr("shop"),wcr);
        //catalog_hd_->setLink(WLink(WLink::InternalPath,"/shop"));
        //catalog_hd_->setRefInternalPath("/shop");
        ////catalog_hd_->setId("catalog_anc");
        ////catalog_hd_->clicked().connect(boost::bind(&Portal::onClick, this, catalog_hd_));

        //catalog_px_ = new WAnchor("/shop", "",m_rightSide);
        //catalog_px_->setLink(WLink(WLink::InternalPath,"/shop"));
        //catalog_px_->setStyleClass("body");
        //catalog_px_->setImage(new WImage("/images/oblojki.png"));
        ////catalog_px_->setId("catalog_btn");
        ////catalog_px_->clicked().connect(boost::bind(&Portal::onClick, this, catalog_px_));

    }
    m_rightSide->refresh();
    return m_rightSide;
}

WWidget *Portal::createHome()
{
//    WTemplate *result = new WTemplate(tr("template.content.mainpage"));
//    result->setId("home1");
//    WWidget *HomePage = result;
    //m_isCatalogMode = false;
#ifdef DEBUG
    log("info")<<"createHome() "<<__LINE__;
#endif
    WContainerWidget* wcHomePage = new  WContainerWidget(root());

    m_content->setId("main_page");
    m_content->setList(true);
    m_content->setInline(true);


    WTemplate *rightSideTmp = new WTemplate(tr("portal.rightside"));
    WWidget *rightSideContent_ = rightSideTmp;
    rightSideContent_->setId("rightside");
    rightSideContent_->setStyleClass("rightside");

    createMainMenu();
    //createRightPanel();
    // Make the menu be internal-path aware.
    //mainMenu_->setInternalPathEnabled("/");
    WContainerWidget* wcMainWrapper = new  WContainerWidget(wcHomePage);
    WContainerWidget* wcMainContent = new  WContainerWidget(wcMainWrapper);
    WContainerWidget* wcMenu = new  WContainerWidget(wcMainContent);
    WContainerWidget* wcMainClear = new  WContainerWidget(wcMainContent);
    wcMainClear->setStyleClass("mainclear clear_inside");
    WContainerWidget* wcMainContent2 = new  WContainerWidget(wcMainClear);
    wcMainContent2->setStyleClass("maincontent");
    WContainerWidget* wcContent = new  WContainerWidget(wcMainContent2);
    wcContent->setId("content");
    //wcContent->setStyleClass("content");
    WContainerWidget* wcBodyContent = new  WContainerWidget(wcContent);
    wcBodyContent->setStyleClass("bodycontent clear_inside");

    sideBarContent_ = new WContainerWidget();
    //wcMenu->setId("menutop");
    sideBarContent_->setId("sideBar");
    wcMainWrapper->setId("main_wrapper");
    wcMainWrapper->setStyleClass("body");
    wcMainContent->setId("main_content");
    wcMainContent->setStyleClass("centered");

    wcMenu->addWidget(navigation_);
    wcMainContent->addWidget(sideBarContent_);
    wcBodyContent->addWidget(m_content);
    wcMainClear->addWidget(rightSideContent_);
    //wcMainContent->addWidget(m_content);
    //wcMainContent->addWidget(rightSideContent_);



    rightSideTmp->bindWidget("catalog",m_rightSide);

    if(m_isTerminalMode)
        wcMenu->hide();
//    result->bindWidget("menu", wcMenu);
//    result->bindWidget("contents", m_content);
//    result->bindWidget("sidebar", sideBarContent_);
//    result->bindWidget("rightside",rightSideContent_);
    wcHomePage->setId("homePage");
#ifdef DEBUG
    log("info")<<"createHome() "<<__LINE__;
#endif
    return wcHomePage;
}

WWidget *Portal::createCatalog()
{
    //root()->setId("catalog");
#ifdef DEBUG
    log("info") << "createCatalog() "<<__LINE__;
#endif

    WContainerWidget *wc = new WContainerWidget();
    int maxPages = session_.maxPage("");

    int currentPage = atoi(internalPathNextPart("/shop/").c_str());
    if (currentPage <= 0)
        currentPage = 1;
#ifdef DEBUG
    log("info")<<"******** CURRENT PAGE "<<currentPage<<"  "<<__LINE__;
#endif
    setInternalPath((WString("/shop/{1}/").arg(currentPage)).toUTF8().c_str());

    m_upPanel = new NavigationPanel(maxPages,currentPage);
    wc->addWidget(m_upPanel);

    bool createHeader = false;
    if(SearchPanel *search = dynamic_cast<SearchPanel*>(root()->find("poisk_1")))
        if(!search->getSearchText().empty())
            createHeader = true;
    if(WLineEdit *search = dynamic_cast<WLineEdit*>(root()->find("poisk")))
        if(!search->text().empty())
            createHeader = true;
    if(createHeader)
    {
        WString str = WString("<H3>{1}</H3>").arg(session_.getPageHeader(currentPage));
        m_pageHeader = new WText(str);
        wc->addWidget(m_pageHeader);
    }


    m_upPanel->onPagechanged().connect(this,&Portal::onPageChanged);
    wc->addWidget( m_catalogView = new CatalogView(currentPage));

    m_downPanel = new NavigationPanel(maxPages,currentPage);
    wc->addWidget(m_downPanel);
    m_downPanel->onPagechanged().connect(this,&Portal::onPageChanged);
    //m_isCatalogMode = true;
    return wc;
}


WContainerWidget *Portal::createFooter()
{
    WContainerWidget *footer = new WContainerWidget();
    footer->setStyleClass("footer");
    WContainerWidget *footer_bg = new WContainerWidget(footer);
    footer_bg->setStyleClass("footer_bg");
    /*
        WContainerWidget *centered = new WContainerWidget(footer_bg);
        centered->setStyleClass("centered");
        WContainerWidget *call_back = new WContainerWidget(centered);
        centered->setStyleClass("call_back");
        //anchor->decorationStyle().
        WAnchor *anchor = new WAnchor("/private/messages",call_back);
        anchor->setLink(WLink(WLink::InternalPath,("/private/messages/send")));
        anchor->setStyleClass("corner_kick");


        new WTemplate("<ins class = \"i1\"></ins><ins class = \"i2\"></ins><ins class = \"i3\"></ins><ins class = \"i4\"></ins>",anchor);//->setStyleClass("i1");
        new WText(tr("portal.callback"),anchor);
    */
    //new WText(WString("version {1}  status {2}  build {3}").arg(AutoVersion::_FULLVERSION_STRING).arg(AutoVersion::_STATUS).arg((int)AutoVersion::_BUILDS_COUNT),footer);
    //new WTemplate(WString("<ins class="i1"></ins><ins class="i2"></ins><ins class="i3"></ins><ins class="i4"></ins>обратная связь",UTF8),achor);
    //WTemplate *result = new WTemplate(tr("template.content.footer"), root());
    //WWidget* footer = result;
    //footer->hide();

    return footer;

}

void Portal::onReportActivate()
{
#ifdef DEBUG
    log("Activate signall" ) << "Report";
#endif
}

void Portal::setLanguage(int index)
{
    if (m_wHeader)
    {
        const Lang& l = languages[index];

        //setLocale(l.code_);

        std::string langPath = l.path_;
        mainMenu_->setInternalBasePath(langPath);
        //examplesMenu_->setInternalBasePath(langPath + "examples");
        /*
        BlogView *blog = dynamic_cast<BlogView *>(findWidget("blog"));
        if (blog)
          blog->setInternalBasePath(langPath + "blog/");
        updateTitle();
        */
        language_ = index;
    }
}

WWidget *Portal::linkSourceBrowser(const std::string& example)
{
    /**
     * Instead of using a WAnchor, which will not progress properly because
     * it is wrapped with wrapView() (-- should we not fix that?), we use
     * a WText which contains an anchor, and enable internal path encoding.
     */
    std::string path = "#/" + SHOP_INTERNAL_PATH + "/" + example;
    WText *a = new WText(tr("source-browser-link").arg(path));
    a->setInternalPathEncoding(true);
    return a;
}

void Portal::setLanguageFromPath()
{
    std::string langPath = internalPathNextPart("/");

    if (langPath.empty())
        langPath = '/';
    else
        langPath = '/' + langPath + '/';

    int newLanguage = 0;

    for (unsigned i = 0; i < languages.size(); ++i)
    {
        if (languages[i].path_ == langPath)
        {
            newLanguage = i;
            break;
        }
    }

    if (newLanguage != language_)
        setLanguage(newLanguage);
}

void Portal::updateTitle()
{
    if (mainMenu_->currentItem())
    {
        setTitle(mainMenu_->currentItem()->text());
        /*
        log("info")<<"*********** ITEM INDEX "<<mainMenu_->currentIndex();
        if(mainMenu_->currentIndex()==1){
            domRoot()->setHeight(WLength(100,WLength::Percentage));
        }else{
            domRoot()->setHeight(WLength::Auto);
        }
        domRoot()->update();
        if(m_footer)
        {
            //root()->removeWidget(m_footer);
            //m_footer = createFooter();
            //root()->addWidget(m_footer);



        }
        */

        for(auto i=folderIdNameMap_.begin(); i!=folderIdNameMap_.end(); ++i)
        {
            if(WWidget *w = i->second)
            {
                w->removeStyleClass("nav");
            }
        }
    }
}

void Portal::logInternalPath(const std::string& path)
{
    // simulate an access log for the interal paths
    //std::cout <<WString(path)<<std::end;
    //log("info")<<path;
    // If this goes to /src, we need to invoke google analytics method too
//    if (path.size() >= 4 && path.substr(0, 4) == "/src")
//    {
//    }
    googleAnalyticsLogger();

}

void Portal::chatSetUser(const WString& userName)
{
    WApplication::instance()->doJavaScript
    ("if (window.chat) "
     "try {"
     """window.chat.emit(window.chat, 'login', "
     ""                   "" + userName.jsStringLiteral() + "); "
     "} catch (e) {"
     """window.chatUser = " + userName.jsStringLiteral() + ";"
     "}"
     "else "
     """window.chatUser = " + userName.jsStringLiteral() + ";");
}

WWidget *Portal::wrapView(WWidget *(Portal::*createWidget)())
{
    return makeStaticModel(boost::bind(createWidget, this));
}

std::string Portal::href(const std::string& url, const std::string& description)
{
    return "<a href=\"" + url + "\" target=\"_blank\">" + description + "</a>";
}


void Portal::updateIntoduction(WApplication *app)
{

//log("info")<<__LINE__;
    std::string orderId = "";
    std::string realizationId = "";

    WStandardItemModel *model;

    WString organization;//
    WString phone;//
    WString email;//
    WString discount;//
    WString userId;
    WString userCode;
    std::string location;

    boost::this_thread::sleep(boost::posix_time::milliseconds(150));
    userId = WString::fromUTF8(boost::any_cast<std::string>(session_.userProperty("Идентификатор 1С")));
    userCode = WString::fromUTF8(boost::any_cast<std::string>(session_.userProperty("Код 1С")));
    boost::this_thread::sleep(boost::posix_time::milliseconds(50));
    location = boost::any_cast<std::string>(session_.userProperty("Регион"));

    MsSql7 *sql = new MsSql7(new WStandardItemModel(0, 0,this));
    sql->connect(std::string("infotex_"+location).c_str(),"sa","sa");

    /// 2       3           4       5   6           7           8                       9              10   11      12      13          14          15              16      17              18          19
    /// ЭлПочта МулАдрес Скидка1 Бонус БонусГород РасчетБонуса НаименованиеМенеджера ПолнНаименование ИНН Телефоны ЮрАдрес ФактАдрес СхемаОплаты ЛимитБезопасности Код НаименованиеФирма Пользователь Пароль
    //log("info")<<" USER char id "<<userId;



    boost::this_thread::sleep(boost::posix_time::milliseconds(70));

    // Get the application update lock to update the user-interface
    // with a progress indication.

//log("info")<<__LINE__;
    WTemplate *result = dynamic_cast<WTemplate*>(root()->find("introduction"));
    if(result)
    {

        boost::this_thread::sleep(boost::posix_time::milliseconds(70));
        {
            WApplication::UpdateLock uiLock(app);
            if (uiLock)
            {
                organization += WString::fromUTF8(boost::any_cast<std::string>(session_.userProperty("Форма собственности")));
                organization += " ";
                organization += WString::fromUTF8(boost::any_cast<std::string>(session_.userProperty("Наименование")));
                phone = WString::fromUTF8(boost::any_cast<std::string>(session_.userProperty("Телефон")));
                email = WString::fromUTF8(boost::any_cast<std::string>(session_.userProperty("eMail")));
                discount = boost::lexical_cast<std::string>(boost::any_cast<double>(session_.userProperty("Скидка")));
                result->bindString("organization",organization);
                result->bindString("person","");
                result->bindString("phone",phone);
                result->bindString("email",email);
                if(!boost::any_cast<bool>(session_.userProperty("Не отображать скидку")))
                    result->bindString("conditions",WString::fromUTF8("Скидка ")+discount);
                else
                    result->bindString("conditions","");
                result->bindString("userinfo.update",WString::fromUTF8("Обновить"));
                app->triggerUpdate();
            }
        }
//log("info")<<__LINE__;

        boost::this_thread::sleep(boost::posix_time::milliseconds(70));
        {
            WApplication::UpdateLock uiLock(app);
            if (uiLock)
            {
                WString dogovor = WString("Нет договора",UTF8);
                if(sql->execute(WString("SELECT * FROM Web_dannKontr2('{1}')").arg(userId)))
                    dogovor = "Unable to connect SQL server !";
                else
                {
                    model = sql->getModel();
                    if(model->rowCount())
                        if(model->item(0, 2)->text().trim()!="")
                            dogovor = model->item(0, 2)->text()+WString(" - ")+WString(model->item(0, 3)->text());
                }
                result->bindString("ofert",dogovor);
                app->triggerUpdate();
            }
        }

        boost::this_thread::sleep(boost::posix_time::milliseconds(80));
        {
            WApplication::UpdateLock uiLock(app);
            if (uiLock)
            {
                WString unPrintedDocs = WString("Нет",UTF8);
#ifndef GSOAP

                if(sql->execute(WString("SELECT * FROM Web_getUserDocs('{1}')").arg(userId))) // долги по непропечатанным документа 1
                    unPrintedDocs = "Unable to connect SQL server !";
                else
                {
                    model = sql->getModel();
                    if(model->rowCount())
                        unPrintedDocs = WString("Есть",UTF8);
                }
#else
                auto unbackedDocs = m_soapClient->getUnbackedDocs(userCode.toUTF8());
                if(unbackedDocs.begin() != unbackedDocs.end())
                    unPrintedDocs = WString("Есть",UTF8);
#endif

                result->bindString("printed",unPrintedDocs);
                app->triggerUpdate();
            }
        }


        boost::this_thread::sleep(boost::posix_time::milliseconds(70));
        {
            WApplication::UpdateLock uiLock(app);
            if (uiLock)
            {
                WString lastImplementation = WString("Отгрузок не было",UTF8);
#ifndef GSOAP

                if(sql->execute(WString("SELECT * FROM Web_dannKontr7('{1}')").arg(userId)))// Последняя отгрузка
                    lastImplementation = "Unable to connect SQL server !";
                else
                {
                    model = sql->getModel();
                    if(model->rowCount())
                    {
                        char number[50];
                        sprintf(number,"%.2f",atof(model->item(0, 4)->text().toUTF8().c_str()));
                        lastImplementation = model->item(0, 3)->text()+WString(" на сумму ",UTF8)+WString("{1} ").arg(number);
                        realizationId = model->item(0, 0)->text().toUTF8();
                    }

                }

#else
                std::vector<std::vector<WString> > lastOrders = m_soapClient->getLastShipment(userCode.toUTF8());

                if(lastOrders.begin()==lastOrders.end())
                    lastImplementation = WString::fromUTF8("Нет данных");
                else
                {
                    std::vector<WString> order = *lastOrders.begin();
                    lastImplementation = order[1]+WString::fromUTF8(" на сумму ")+order[2];
                    realizationId = order[0].toUTF8();

                }
#endif
                WAnchor *aMutual = new WAnchor("/reports/dept_structure", WString("Подробнее:",UTF8));
                aMutual->setLink(WLink(WLink::InternalPath,"/reports/dept_structure"));

                WAnchor *aLastUpload = new WAnchor("javascript:void(0)", WString("Подробнее:",UTF8));
                aLastUpload->setObjectName(realizationId);
                aLastUpload->setId("show_realization");
                aLastUpload->clicked().connect(boost::bind(&Portal::onClick, this, aLastUpload));

                //result->bindWidget("userinfo.lastupload",aLastUpload);
                result->bindString("lastupload",lastImplementation);
                result->bindWidget("userinfo.mutual",aMutual);
                app->triggerUpdate();

            }
        }

        boost::this_thread::sleep(boost::posix_time::milliseconds(70));
        {
            WApplication::UpdateLock uiLock(app);
            if (uiLock)
            {

                WString lastOrder = WString("Заказов не было",UTF8);
#ifndef GSOAP

                if(sql->execute(WString("SELECT * FROM Web_getLastOrder('{1}')").arg(userId)))// Сумма просроченная
                    lastOrder = "Unable to connect SQL server !";
                else
                {
                    model = sql->getModel();

                    if(model->rowCount())
                    {
                        char number[50];
                        sprintf(number,"%.2f",atof(model->item(0, 4)->text().toUTF8().c_str()));
                        lastOrder = model->item(0, 3)->text()+WString(" на сумму ",UTF8)+WString("{1} ").arg(number);
                        orderId = model->item(0, 0)->text().toUTF8();
                    }
                }

#else
                std::vector<std::vector<WString> > lastOrders = m_soapClient->getLastOrder(userCode.toUTF8());

                if(lastOrders.begin()==lastOrders.end())
                    lastOrder = WString::fromUTF8("Нет данных");
                else
                {
                    std::vector<WString> order = *lastOrders.begin();
                    lastOrder = order[1]+WString::fromUTF8(" на сумму ")+order[2];
                    orderId = order[0].toUTF8();
                }
#endif

                WAnchor *aLastOrder = new WAnchor("javascript:void(0)", WString("Подробнее:",UTF8));
                aLastOrder->setObjectName(orderId);
                aLastOrder->setId("show_order");
                aLastOrder->clicked().connect(boost::bind(&Portal::onClick, this, aLastOrder));

                result->bindString("lastorder",lastOrder);
                //result->bindWidget("userinfo.lastorder",aLastOrder);
                app->triggerUpdate();
            }
        }

        boost::this_thread::sleep(boost::posix_time::milliseconds(70));
        {
            WApplication::UpdateLock uiLock(app);
            if (uiLock)
            {

                WString totalOverdue = WString("Нет данных",UTF8);
                WString currentOverdue = WString("Нет данных",UTF8);
                WString OverdueDays = "0";
#ifndef GSOAP

                if(sql->execute(WString("SELECT * FROM Web_getUserDept('{1}', 1)").arg(userId)))// Сумма просроченная

                    totalOverdue = "Unable to connect SQL server !";
                else
                {
                    model = sql->getModel();

                    if(model->rowCount())
                    {
                        totalOverdue = model->item(0, 1)->text();
                        currentOverdue = model->item(0, 0)->text();
                        if(totalOverdue != "0")
                            OverdueDays = model->item(0, 2)->text();
                    }
                }

#else
                std::vector<WString> userDept = m_soapClient->getUserDept(userCode.toUTF8());

                if(userDept.begin()==userDept.end())
                    totalOverdue = "Unable to connect SQL server !";
                else
                {
                    totalOverdue = userDept[2];
                    currentOverdue = userDept[1];
                    if(totalOverdue != "0")
                        OverdueDays = userDept[3];

                }
#endif
                result->bindString("currency",totalOverdue);
                result->bindString("daycount",OverdueDays);
                result->bindString("mutual",currentOverdue);
                app->triggerUpdate();
            }
        }

        boost::this_thread::sleep(boost::posix_time::milliseconds(70));
        {
            WApplication::UpdateLock uiLock(app);
            if (uiLock)
            {
                WText *aChange = new WText(tr("userinfo.change"));
                aChange->setObjectName("userprofile");
                aChange->show();
                aChange->clicked().connect(boost::bind(&Portal::onClick, this, aChange));
                result->bindWidget("userinfo.change",aChange);
                app->triggerUpdate();
            }
        }

    }


    WApplication::UpdateLock uiLock(app);
    if (uiLock)
    {
        if(result)
        {
            WAnchor *update_kick = new WAnchor("javascript: void(0)", tr("update"));
            update_kick->setObjectName("update_userinfo");
            update_kick->setStyleClass("debt");
            update_kick->clicked().connect(boost::bind(&Portal::onClick, this, update_kick));
            result->bindWidget("userinfo.update",update_kick);

        }
        app->triggerUpdate();
        // Disable server push
        app->enableUpdates(false);
    }

//log("info")<<__LINE__;
    delete sql;

}


WWidget *Portal::introduction()
{
#ifdef DEBUG
    log("info")<<" introduction() "<<__LINE__;
#endif


    WTemplate *result = dynamic_cast<WTemplate*>(root()->find("introduction"));
    if(!result)
    {
        result = new WTemplate(tr("portal.introduction"));
        result->setObjectName("introduction");
    }
    WWidget *userinfoWidget = result;


    result->bindWidget("organization",new WImage("icons/ajax-loader.gif"));
    result->bindString("person","");

    result->bindWidget("phone",new WImage("icons/ajax-loader.gif"));
    result->bindWidget("email",new WImage("icons/ajax-loader.gif"));
    result->bindWidget("ofert",new WImage("icons/ajax-loader.gif"));
    result->bindWidget("conditions",new WImage("icons/ajax-loader.gif"));

    result->bindWidget("currency",new WImage("icons/ajax-loader.gif"));
    result->bindString("daycount","");
    result->bindWidget("mutual",new WImage("icons/ajax-loader.gif"));
    result->bindWidget("printed",new WImage("icons/ajax-loader.gif"));
    result->bindWidget("lastorder",new WImage("icons/ajax-loader.gif"));
    result->bindWidget("lastupload",new WImage("icons/ajax-loader.gif"));

    result->bindString("userinfo.mutual","");
    result->bindString("userinfo.change","");
    result->bindString("userinfo.lastorder","");
    result->bindString("userinfo.lastupload","");

    WAnchor *update_kick = new WAnchor("javascript: void(0)", tr("update"));
    update_kick->setObjectName("update_userinfo");
    update_kick->setStyleClass("debt");
    update_kick->clicked().connect(boost::bind(&Portal::onClick, this, update_kick));

    result->bindWidget("userinfo.update",update_kick);

    enableUpdates(true);
    workThread_  = boost::thread(boost::bind(&Portal::updateIntoduction, this, this));

//log("info")<<__LINE__;

    return userinfoWidget;
}

void Portal::userProfile()
{

    WDialog dialog(WString("Изменение персональных данных",UTF8));
    dialog.setClosable(true);

    /*
    MsSql7 *sql = new MsSql7(new WStandardItemModel(0, 0,this));
    sql->connect(std::string("infotex_"+session_.user()->location).c_str(),"sa","sa");
    if(sql->execute(WString("SELECT top 6 * FROM Web_getUserProperty('{1}')").arg(session_.userCharId())))// Сумма просроченная
    {
        delete sql;
        return;
    }
    WStandardItemModel *model = sql->getModel();
    */
    WStandardItem *item = 0;
    int row = 0;
    WStandardItemModel *model = new WStandardItemModel(7,3);

    item = new WStandardItem();
    item->setText("8");
    model->setItem(row, 0,item);
    item->setText(WString::fromUTF8(boost::any_cast<std::string>(session_.userProperty("Наименование"))));
    model->setItem(row++, 1,item);

    item = new WStandardItem();
    item->setText("0");
    model->setItem(row, 0,item);
    item->setText("");
    model->setItem(row++, 1,item);

    item = new WStandardItem();
    item->setText("0");
    model->setItem(row, 0,item);
    item->setText("1");
    model->setItem(row++, 1,item);

    item = new WStandardItem();
    item->setText("0");
    model->setItem(row, 0,item);
    item->setText("");
    model->setItem(row++, 1,item);

    item = new WStandardItem();
    item->setText("11");
    model->setItem(row, 0,item);
    item->setText(WString::fromUTF8(boost::any_cast<std::string>(session_.userProperty("Телефон"))));
    model->setItem(row++, 1,item);

    item = new WStandardItem();
    item->setText("13");
    model->setItem(row, 0,item);
    item->setText(WString::fromUTF8(boost::any_cast<std::string>(session_.userProperty("Фактический адрес"))));
    model->setItem(row++, 1,item);

    item = new WStandardItem();
    item->setText("0");
    model->setItem(row, 0,item);
    item->setText("");
    model->setItem(row++, 1,item);




    UserProfile *profile = new UserProfile(dialog.contents());

    profile->setModel(model);

    WPushButton submit(tr("submit"), dialog.contents());
    new WText(" ",dialog.contents());
    WPushButton cancell(tr("cancel"), dialog.contents());

    submit.clicked().connect(&dialog, &WDialog::accept);
    cancell.clicked().connect(&dialog, &WDialog::reject);
    if (dialog.exec() == WDialog::Accepted)
    {
        //setStatus("Welcome, " + edit.text());

    }
    //delete sql;
}

WWidget *Portal::userCart()
{
    WStandardItem *item = 0;
    int row = 0;
    WStandardItemModel *model = new WStandardItemModel(7,3);

    item = new WStandardItem();
    item->setText("8");
    model->setItem(row, 0,item);
    item->setText(WString::fromUTF8(boost::any_cast<std::string>(session_.userProperty("Наименование"))));
    model->setItem(row++, 1,item);

    item = new WStandardItem();
    item->setText("0");
    model->setItem(row, 0,item);
    item->setText("");
    model->setItem(row++, 1,item);

    item = new WStandardItem();
    item->setText("0");
    model->setItem(row, 0,item);
    item->setText("1");
    model->setItem(row++, 1,item);

    item = new WStandardItem();
    item->setText("0");
    model->setItem(row, 0,item);
    item->setText("");
    model->setItem(row++, 1,item);

    item = new WStandardItem();
    item->setText("11");
    model->setItem(row, 0,item);
    item->setText(WString::fromUTF8(boost::any_cast<std::string>(session_.userProperty("Телефон"))));
    model->setItem(row++, 1,item);

    item = new WStandardItem();
    item->setText("13");
    model->setItem(row, 0,item);
    item->setText(WString::fromUTF8(boost::any_cast<std::string>(session_.userProperty("Фактический адрес"))));
    model->setItem(row++, 1,item);

    item = new WStandardItem();
    item->setText("0");
    model->setItem(row, 0,item);
    item->setText("");
    model->setItem(row++, 1,item);




    UserProfile *profile = new UserProfile();

    profile->setModel(model);
    return profile;
}

WWidget *Portal::operators()
{

#ifdef DEBUG
    log("info")<<" operators() "<<__LINE__;
#endif
    //log("info")<<"*** User "<< session_.userId()<<"   "<<session_.userCharId();

    //WString userId = session_.userCharId();

    WString userId;
    std::string location;

    boost::this_thread::sleep(boost::posix_time::milliseconds(50));
    userId = WString::fromUTF8(boost::any_cast<std::string>(session_.userProperty("Идентификатор 1С")));
    boost::this_thread::sleep(boost::posix_time::milliseconds(50));
    location = boost::any_cast<std::string>(session_.userProperty("Регион"));


    //MsSql7 *sql = new MsSql7(new WStandardItemModel(0, 0,this));
    //sql->connect(std::string("infotex_"+location).c_str(),"sa","sa");
    /// 0           1           2       3       4   5       6
    /// ВидЗаписи Наименование Телефон Почта Город Скайп isq
    //if(sql->execute(WString("SELECT * FROM Web_getOperatorInfo('{1}')").arg(userId)))
    //    return new WText("Unable to connect SQL server !");
    //WStandardItemModel *model = sql->getModel();
    //log("info")<<" operators() "<<__LINE__;
    std::string operatorId = boost::any_cast<std::string>(session_.userProperty("Оператор"));
    std::string managerId = boost::any_cast<std::string>(session_.userProperty("Менеджер"));

    dbo::Transaction transaction(session_.getSession());
    dbo::ptr<User> operatorUser;
    if(operatorId.end() != operatorId.begin())
        operatorUser = session_.getSession().find<User>().where("ref = ?").bind(operatorId);

    std::string email = "—";
    std::string name = "—";
    std::string phone = "—";
    if(operatorUser)
        for(auto i = operatorUser->userProperty.begin(); i!=operatorUser->userProperty.end(); ++i)
        {
            log("OPERATOR")<<(*i)->propertyField->fieldId;
            switch((*i)->propertyField->fieldId)
            {
            case 4:
                email = (*i)->simpleValue;;
                break;
            case 8:
                name = (*i)->simpleValue;;
                break;
            case 11:
                phone =(*i)->simpleValue;;
                break;

            }
        }
    //WString(model->item(0, 1)->text(),UTF8)
    WString templatestr = WString("<h1>Личный кабинет</h1>",UTF8);
    //if(model->rowCount())
    {
        templatestr += WString("<h2>Ваш оператор:</h2>",UTF8);
        templatestr += WString(tr("portal.operators"))
                       .arg(WString(name,UTF8))
                       .arg(WString(phone,UTF8))
                       .arg(WString(email,UTF8))
                       .arg(WString("—",UTF8))
                       .arg(WString("—",UTF8))
                       .arg(WString("—",UTF8));
    }


    dbo::ptr<User> managerUser;
    if(managerId.end() != managerId.begin())
        managerUser = session_.getSession().find<User>().where("ref = ?").bind(managerId);

    if(managerUser)
        for(auto i = managerUser->userProperty.begin(); i!=managerUser->userProperty.end(); ++i)
        {
            log("MANAGER")<<(*i)->propertyField->fieldId;
            switch((*i)->propertyField->fieldId)
            {
            case 4:
                email = (*i)->simpleValue;;
                break;
            case 8:
                name = (*i)->simpleValue;;
                break;
            case 11:
                phone =(*i)->simpleValue;;
                break;

            }
        }
    //if(model->rowCount() >1)
    {
        templatestr += WString("<h2>Ваш менеджер:</h2>",UTF8);
        templatestr += WString(tr("portal.operators"))
                       .arg(WString(name,UTF8))
                       .arg(WString(phone,UTF8))
                       .arg(WString(email,UTF8))
                       .arg(WString("—",UTF8))
                       .arg(WString("—",UTF8))
                       .arg(WString("—",UTF8));
    }

    templatestr += WString("<h2>Администратор сайта:</h2>",UTF8);
    templatestr += WString(tr("portal.operators"))
                   .arg(WString("Администратор",UTF8))
                   .arg(WString("—",UTF8))
                   .arg(WString("luban@infotex.org",UTF8))
                   .arg(WString("—",UTF8))
                   .arg(WString("—",UTF8))
                   .arg(WString("Санкт-Петербург",UTF8));

    //delete sql;
    return new WText(templatestr);
}

WWidget *Portal::reports()
{

#ifdef DEBUG
    log("info")<<" reports() "<<__LINE__;
#endif


    WContainerWidget *wc = new WContainerWidget();
    new WText("<h2>"+tr("reports")+"</h2><br/>",wc);

    // dept_structure
    //result->setInternalPathEncoding(true);
    //WAnchor *update_kick =
    (new WAnchor("/reports/mutual", tr("mutual"),wc))->setLink(WLink(WLink::InternalPath,"/reports/mutual"));
    new WText("<br/>",wc);
    (new WAnchor("/reports/order_journal", tr("order_journal"),wc))->setLink(WLink(WLink::InternalPath,"/reports/order_journal"));
    new WText("<br/>",wc);
    (new WAnchor("/reports/dept_structure", tr("dept_structure"),wc))->setLink(WLink(WLink::InternalPath,"/reports/dept_structure"));
    new WText("<br/>",wc);
    (new WAnchor("/reports/sales", tr("sales"),wc))->setLink(WLink(WLink::InternalPath,"/reports/sales"));
    //new WText("<br/>",wc);
    //(new WAnchor("/reports/orders", tr("orders"),wc))->setLink(WLink(WLink::InternalPath,"/reports/orders"));
    //new WText("<br/>",wc);
    //(new WAnchor("/reports/messages", tr("messages"),wc))->setLink(WLink(WLink::InternalPath,"/reports/messages"));

    return wc;
}

WWidget *Portal::information()
{
#ifdef DEBUG
    log("info")<<" information() "<<__LINE__;
#endif

    WContainerWidget *wc = new WContainerWidget();
    new WText("<h2>"+tr("information")+"</h2><br/>",wc);

    (new WAnchor("/information/services", tr("services"),wc))->setLink(WLink(WLink::InternalPath,"/information/services"));
    new WText("<br/>",wc);
#ifdef DEBUG
    log("info")<<" information() "<<__LINE__;
#endif
    return wc;
}

WWidget *Portal::terminal()
{
    return new WText();
}

WWidget *Portal::priceEditor()
{
    return new PriceEditor();
}

WWidget *Portal::loadPrice()
{
    return new ImportOrder(ImportOrder::Price);
}


void Portal::setpassword()
{

    //log("info")<<"setpassword() "<<__LINE__;
    WContainerWidget *pwdDiv = new WContainerWidget(root());
    pwdDiv->setStyleClass("passwdDialog");
    WContainerWidget *content = new WContainerWidget(pwdDiv);
    WContainerWidget *buttonsDiv = new WContainerWidget(content);
    content->setStyleClass("content");
    WLabel *label;
    label= new WLabel(WString("Введите пароль:",UTF8),buttonsDiv);
    WLineEdit *m_password = new WLineEdit(buttonsDiv);
    m_password->setStyleClass("textInput");
    m_password->setEchoMode(WLineEdit::Password);
    m_password->setObjectName("passwordEdit");
    label->setBuddy(m_password);
    new WBreak(buttonsDiv);
    label = new WLabel(WString("Повторите ввод:",UTF8),buttonsDiv);
    WLineEdit *m_confirm = new WLineEdit(buttonsDiv);
    m_confirm->setStyleClass("textInput");
    m_confirm->setObjectName("passwordConfirm");
    m_confirm->setEchoMode(WLineEdit::Password);
    //m_confirm->changed().connect(boost::bind(&Portal::confirmPassword,this,m_password,m_confirm));
    label->setBuddy(m_confirm);
    new WBreak(buttonsDiv);
    new WBreak(buttonsDiv);
    buttonsDiv->setStyleClass("buttons");
    WPushButton *OkBtn = new WPushButton(tr("save"),buttonsDiv);
    OkBtn->clicked().connect(boost::bind(&Portal::onClick, this, m_password));
    WPushButton *CancelBtn = new WPushButton(tr("cancel"),buttonsDiv);
    CancelBtn->clicked().connect(boost::bind(&Portal::onClick, this, CancelBtn));
    CancelBtn->setObjectName("cancelBtn");
    pwdDiv->show();
    //return pwdDiv;
}

void Portal::requestPassword()
{
#ifdef DEBUG
    log("info")<<"void Portal::requestPassword() "<<__LINE__;
#endif
    WContainerWidget *pwdDiv;
    pwdDiv = (WContainerWidget*)root()->find("passwordRequestDialog");
    //log("info")<<pwdDiv;
    if(pwdDiv)
    {
        //m_content->setCurrentWidget((WWidget*)pwdDiv);
        pwdDiv->show();
    }

    else
    {
        pwdDiv = new WContainerWidget(root());
        pwdDiv->setStyleClass("passwdDialog");
        pwdDiv->setObjectName("passwordRequestDialog");
        WContainerWidget *content = new WContainerWidget(pwdDiv);
        WContainerWidget *buttonsDiv = new WContainerWidget(content);
        content->setStyleClass("content");
        WLabel *label;
        label= new WLabel(WString("Введите пароль:",UTF8),buttonsDiv);
        WLineEdit *m_password = new WLineEdit(buttonsDiv);
        m_password->setStyleClass("textInput");
        m_password->setEchoMode(WLineEdit::Password);
        m_password->setObjectName("passwordRequest");
        m_password->enterPressed().connect(boost::bind(&Portal::onClick, this, pwdDiv));
        label->setBuddy(m_password);
        new WBreak(buttonsDiv);
        new WBreak(buttonsDiv);
        buttonsDiv->setStyleClass("buttons");
        WPushButton *OkBtn = new WPushButton(tr("ok"),buttonsDiv);
        OkBtn->clicked().connect(boost::bind(&Portal::onClick, this, pwdDiv));
        WPushButton *CancelBtn = new WPushButton(tr("cancel"),buttonsDiv);
        CancelBtn->clicked().connect(boost::bind(&Portal::onClick, this, CancelBtn));
        CancelBtn->setObjectName("cancelBtn");
        pwdDiv->show();
    }

}

WWidget *Portal::privateDate()
{
#ifdef DEBUG
    log("info")<<" privateDate() "<<__LINE__;
#endif

    WContainerWidget *wc = new WContainerWidget();
    new WText("<h2>"+tr("private")+"</h2><br/>",wc);

    (new WAnchor("/private/introduction", tr("introduction"),wc))->setLink(WLink(WLink::InternalPath,"/private/introduction"));
    new WText("<br/>",wc);
    (new WAnchor("/private/operator", tr("operator"),wc))->setLink(WLink(WLink::InternalPath,"/private/operator"));
    new WText("<br/>",wc);
    (new WAnchor("/private/messages", tr("portal.messages"),wc))->setLink(WLink(WLink::InternalPath,"/private/messages"));

    return wc;
}

WWidget *Portal::orders()
{

    WContainerWidget *wc = new WContainerWidget();
    new WText("<h2>"+tr("orders")+"</h2><br/>",wc);

    (new WAnchor("/orders/current", tr("portal.current_order"),wc))->setLink(WLink(WLink::InternalPath,"/orders/current"));
    new WText("<br/>",wc);
    (new WAnchor("/orders/journal", tr("portal.orders.journal"),wc))->setLink(WLink(WLink::InternalPath,"/orders/journal"));

    return wc;
}

WWidget *Portal::mutual()
{
    return new Reports(NULL,  Reports::Mutual);
}

WWidget *Portal::order_journal()
{
    return new Reports(NULL,  Reports::Orders);
}

WWidget *Portal::dept_structure()
{
    return new Reports(NULL,  Reports::DeptStucture);
}

WWidget *Portal::sales()
{
    return new WText(tr("portal.sales"));
    //return new Reports(NULL,  Reports::GoodsSales);
}

WWidget *Portal::orderDetails()
{
    //if(m_isTerminalMode)
    //    return new WText();
    return new Reports(NULL,  Reports::OrderDetail);
}

WWidget *Portal::ordersJournal()
{
    return new Reports(NULL,  Reports::OrderJournal);
}

void Portal::ordersJournalShow()
{

    //log("info")<<"void Portal::ordersJournalShow() "<<__LINE__;

    Reports *orderJrn = (Reports*)m_content->find("OrderJournal");
    if(orderJrn)
        m_content->removeWidget(orderJrn);
    else
        m_content->addWidget(orderJrn = new Reports(NULL,  Reports::OrderJournal));


    m_content->currentWidget()->hide();
    m_content->setCurrentWidget(orderJrn);
    //m_content->currentWidget()->show();
    //log("info")<<"void Portal::ordersJournalShow() "<<__LINE__;
}

WWidget *Portal::importOrder()
{
    if(m_isTerminalMode)
        return new WText();

    return new ImportOrder(ImportOrder::Order);
}


WWidget *Portal::services()
{
#ifdef DEBUG
    log("info")<<" services() "<<__LINE__;
#endif
    if(m_isTerminalMode)
        return new WText();


    WContainerWidget *wc = new WContainerWidget();
    new WText(tr("portal.services"),wc);
    new WText(tr("portal.services.center"),wc);
//    WTable *servTable = new WTable();
//    servTable->setHeaderCount(1);
//    servTable->elementAt(0,0)->addWidget(new WText(tr("portal.services.header.coll1")));
//    servTable->elementAt(0,1)->addWidget(new WText(tr("portal.services.header.coll2")));
//    servTable->elementAt(0,2)->addWidget(new WText(tr("portal.services.header.coll3")));
//    servTable->elementAt(0,3)->addWidget(new WText(tr("portal.services.header.coll4")));
//    servTable->elementAt(0,4)->addWidget(new WText(tr("portal.services.header.coll5")));
#ifdef DEBUG
    log("info")<<" services() "<<__LINE__;
#endif

    return wc;
}


WWidget *Portal::whatsnew()
{
    WContainerWidget *wc = new WContainerWidget();
    new WText(tr("portal.whatsnew"),wc);
    new WText(tr("portal.whatsnew.center"),wc);
    return wc;
}

WWidget *Portal::loadexcel()
{
    if(m_isTerminalMode)
        return new WText();

    return new WText(tr("portal.loadexcel"));
}

WWidget *Portal::news()
{
#ifdef DEBUG
    log("info")<<" news() "<<__LINE__;
#endif
    if(m_isTerminalMode)
        return new WText();

    WContainerWidget *wc = new WContainerWidget();

    WContainerWidget *actionBorder = new WContainerWidget(wc);
    actionBorder->setStyleClass("border-bottom:1px dotted #DDDDDD");
    wc->setStyleClass("bodycontent clear_inside");
    new WBreak(wc);
    new WText(WString("<h1>Акции и Последние новости</h1>",UTF8),wc);
    new WBreak(wc);
    WTemplate *news = new WTemplate(tr("portal.newsactions"),wc);
    news->setWidth(645);


    return wc;
}

WWidget *Portal::currentOrder()
{
    return new WText(tr("portal.current_order"));
}

WWidget *Portal::messages()
{
    return new Messages(session_.getSession());
}

void Portal::readReleases(WTable *releaseTable)
{
    std::ifstream f((filePrefix() + "releases.txt").c_str());

    releaseTable->clear();

    releaseTable->elementAt(0, 0)
    ->addWidget(new WText(tr("home.download.version")));
    releaseTable->elementAt(0, 1)
    ->addWidget(new WText(tr("home.download.date")));
    releaseTable->elementAt(0, 2)
    ->addWidget(new WText(tr("home.download.description")));

    releaseTable->elementAt(0, 0)->resize(WLength(15, WLength::FontEx),
                                          WLength::Auto);
    releaseTable->elementAt(0, 1)->resize(WLength(15, WLength::FontEx),
                                          WLength::Auto);

    int row = 1;

    while (f)
    {
        std::string line;
        getline(f, line);

        if (f)
        {
            typedef boost::tokenizer<boost::escaped_list_separator<char> >
            CsvTokenizer;
            CsvTokenizer tok(line);

            CsvTokenizer::iterator i=tok.begin();

            std::string fileName = *i;
            std::string description = *(++i);
            releaseTable->elementAt(row, 0)->addWidget
            (new WText(href("http://prdownloads.sourceforge.net/witty/"
                            + fileName + "?download", description)));
            releaseTable->elementAt(row, 1)->addWidget(new WText(*(++i)));
            releaseTable->elementAt(row, 2)->addWidget(new WText(*(++i)));

            ++row;
        }
    }
}


void Portal::openDocTable(std::string docId)
{
    //SELECT * FROM Web_getDocTable(docId)
    WDialog dialog(WString("Просмотр данных отгрузки",UTF8));
    dialog.setClosable(true);

    MsSql7 *sql = new MsSql7(new WStandardItemModel(0, 0,this));

    std::string location = boost::any_cast<std::string>(session_.userProperty("Регион"));


    sql->connect(std::string("infotex_"+location).c_str(),"sa","sa");
    if(sql->execute(WString("SELECT * FROM Web_getAnyDocTable('{1}','{2}') ORDER BY LineNumber").arg(docId).arg(session_.userCharId())))
        return;
    WStandardItemModel *model = sql->getModel();

    DocumentDetails *docdetail = new DocumentDetails(dialog.contents());
    if(model)
        docdetail->setModel(model);

    WContainerWidget *wc = new WContainerWidget(dialog.contents());
    wc->setId("form_div");
    wc->addWidget(docdetail);
    wc->setOverflow(WContainerWidget::OverflowAuto);
    wc->setMaximumSize( WLength::Auto , 400 );
    wc->decorationStyle().setBackgroundColor(WColor(238,238,238));
    docdetail->setId("doc_detail");
    dialog.setResizable(false);


//    WPushButton cancell(tr("cancel"), dialog.contents());
//    cancell.clicked().connect(&dialog, &WDialog::reject);
    if (dialog.exec() == WDialog::Accepted)
    {

    }
    delete sql;

}

const bool Portal::getAccesStatus()
{
    return m_accessStatus;
}


WString Portal::tr(const char *key)
{
    return WString::tr(key);
}

void Portal::googleAnalyticsLogger()
{
    std::string googleCmd =
        "if (window.pageTracker) {"
        """try {"
        ""  "setTimeout(function() {"
        ""  "window.pageTracker._trackPageview(\""
        + environment().deploymentPath() + internalPath() + "\");"
        ""  "}, 1000);"
        """} catch (e) { }"
        "}";
    doJavaScript(googleCmd);
}


void Portal::onPageChanged(int page)
{

#ifdef DEBUG
    log("info") << "onPageChanged(page "<<page<<" sender  "<<sender()<<") "<<__LINE__;
#endif
    if(page)
    {

        //if(SearchPanel *search = dynamic_cast<SearchPanel*>(root()->find("poisk")))
        //{
        //    if(m_catalogView)
        //        m_catalogView->setSearchString(search->getSearchText());
        //}

        int maxPages = session_.maxPage("");
        m_downPanel->setMaxPage(maxPages);
        m_upPanel->setMaxPage(maxPages);

        m_catalogView->setCurrentPage(page);
        m_catalogView->update();
        //if(dynamic_cast<NavigationPanel*>(sender()))
        //{
        //    m_folderPanel->setCurrentPage(page);
        //    m_folderPanel->update();
        //}
        //m_catalogView->update();

        setInternalPath((WString("/shop/{1}/").arg(page)).toUTF8().c_str());

        if(m_downPanel->getCurrentPage() != page)
            m_downPanel->setCurrentPage(page);

        if(m_upPanel->getCurrentPage() != page)
            m_upPanel->setCurrentPage(page);

        if(m_pageHeader)
        {
            WString str = WString("<H3>{1}</H3>").arg(session_.getPageHeader(page));
            ((WText*)m_pageHeader)->setText(str);
        }

    }

}


void Portal::onClick(WWidget* sender)
{
#ifdef DEBUG
    log("info") << "__________________________________________";
    log("info") << "| sender  id "<<WObject::sender()->id();
    log("info") << "| sender  name "<<WObject::sender()->objectName();
    log("info") << "| param  id "<<sender->id();
    log("info") << "| param  name "<<sender->objectName();
    log("info") << "__________________________________________";
#endif


    if(sender->id()==CATALOG)
    {
#ifdef DEBUG
        log("info") << "Current page "<< ((WComboBox*)sender)->currentText();
#endif
        //root()->removeWidget(m_catalogView);
        //m_currentPage = ((WComboBox*)sender)->currentIndex();
        //w->hide();
    }
    else if(sender->objectName()=="userprofile")
    {
        userProfile();
        return;
    }
    else if(sender->objectName()=="update_userinfo")
    {
        introduction();
        return;
    }
    else if(sender->id()=="show_realization" || sender->id()=="show_order")
    {
        openDocTable(sender->objectName());
        return;
    }
    else if(sender->objectName()=="passwordEdit")
    {
        WLineEdit *le = dynamic_cast<WLineEdit*>(sender);
        //WMessageBox::show("Password", le->text(), Ok);
        WContainerWidget *parent = dynamic_cast<WContainerWidget*>(sender->parent()->parent()->parent());
        WLineEdit *cp = dynamic_cast<WLineEdit*>(parent->find("passwordConfirm"));
        if(parent)
        {
            if(cp->text() == le->text())
            {
                parent->hide();
                session_.setUserProperty("Пароль менеджера",boost::any(le->text().toUTF8()));
            }
            else
            {
                WMessageBox::show("Password", WString("Пароли не совпадают. Повторите попытку.",UTF8), Ok);
            }
        }
    }
    else if(sender->objectName()=="orderJournalAnc")
    {
        //if(m_accessStatus)
        ordersJournalShow();
        //else
        //{
        //    requestPassword();
        //}
    }
    else if(sender->objectName() == "passwordRequestDialog")
    {

        sender->hide();
        WLineEdit *pass = dynamic_cast<WLineEdit*>(sender->find("passwordRequest"));
        if(pass)
        {
            if(boost::any_cast<std::string>(session_.userProperty("Пароль менеджера")) == pass->text().toUTF8())
            {
                ordersJournalShow();
                m_accessStatus = true;
            }
            else
            {
                m_accessStatus = false;
                WMessageBox::show("Password", WString("Не верный пароль ",UTF8), Ok);
            }

        }
    }
    else if(sender->objectName() == "previewbtn")
    {
        Reports *report = dynamic_cast<Reports*>(WApplication::instance()->root()->find("OrderDetail"));
        if(report)
        {
            //log("info")<<"Update widget report";
            m_content->currentWidget()->hide();
            report->update();
            m_content->setCurrentWidget(report);
        }
        else
        {
            //log("info")<<"Add widget report";
            WWidget *report;
            m_content->currentWidget()->hide();
            m_content->addWidget(report = new Reports(NULL,  Reports::OrderDetail));
            m_content->setCurrentWidget(report);
        }

    }
    else if(sender->objectName()=="cancelBtn")
    {
        //WPushButton *btn = dynamic_cast<WPushButton*>(sender);
        //WMessageBox::show("Password", "Cancel", Ok);
        WContainerWidget *parent = dynamic_cast<WContainerWidget*>(sender->parent()->parent()->parent());
        if(parent)
            parent->hide();
    }

    WImage *image = dynamic_cast<WImage*>(sender);
    if(image)
    {
        if(image->objectName() == "imageView")
        {
#ifdef DEBUG
            log("info") << "*** IMAGE VIEW***";
#endif
            //image->parent()->parent()->setHidden(true);
        }
        else
        {
#ifdef DEBUG
            log("info") << "*** IMAGE ***";
#endif
            WDialog *imgDia = new WDialog(image->objectName());
            imgDia->setClosable(true);
            imgDia->contents()->setStyleClass("clear_inside");
            imgDia->setModal(true);
            WContainerWidget *wc = new WContainerWidget();
            WImage *img = new WImage("/images/action/"+image->objectName());
            img->setMaximumSize( WLength(98,WLength::Percentage) , WLength(100,WLength::Percentage) );
            img->resize(WLength::Auto,WLength::Auto);
            wc->addWidget(img);
            //wc->setStyleClass("clear_inside");
            //wc->setOverflow(WContainerWidget::OverflowAuto);
            //wc->setMaximumSize( WLength::Auto, WLength(80,WLength::Percentage));//WLength(80,WLength::Percentage) );
            img->setObjectName("imageView");
            imgDia->contents()->addWidget(wc);
            imgDia->setResizable(true);
            imgDia->show();
            img->clicked().connect(boost::bind(&Portal::onClick, this, imgDia));

        }

    }
    WDialog *imgDia = dynamic_cast<WDialog*>(sender);
    if(imgDia)
    {
#ifdef DEBUG
        log("info") << "*** IMAGE VIEW DIALOG ***";
#endif
        WContainerWidget *wc = dynamic_cast<WContainerWidget*>(imgDia->contents()->widget(0));
        //WContainerWidget *wc = imgDia->contents()->widget(0);
        if(wc)
        {
            wc->setOverflow(WContainerWidget::OverflowAuto);
            wc->setMaximumSize( WLength::Auto, 600);//WLength(80,WLength::Percentage) );
            WWidget *img = wc->widget(0);
            img->resize(WLength::Auto,WLength::Auto);
            img->setMaximumSize(WLength(100,WLength::Percentage),WLength(100,WLength::Percentage));
        }

    }


    //m_wHomePage->hide();
}

void Portal::onSearch(WString searchStr)
{

    int currentPage = session_.findCode(searchStr);
    bool findByCode = false;
    if(currentPage)
        findByCode = true;
#ifdef DEBUG
    log("info")<<" PAGE "<<currentPage<<" Search str "<<searchStr;
#endif
    if(findByCode)
    {
        //currentPage--;
        int maxPages = session_.maxPage("");
        m_downPanel->setMaxPage(maxPages);
        m_upPanel->setMaxPage(maxPages);

        m_catalogView->setSearchString(searchStr,findByCode);
        m_catalogView->setCurrentPage(currentPage);
        m_catalogView->update();
        m_downPanel->setCurrentPage(currentPage);
        m_upPanel->setCurrentPage(currentPage);
        WApplication::instance()->setInternalPath((WString("/shop/{1}/").arg(currentPage)).toUTF8().c_str());
    }
    else
    {
        int maxPages = session_.maxPage(searchStr)-1;
        m_downPanel->setMaxPage(maxPages);
        m_upPanel->setMaxPage(maxPages);
        m_catalogView->setSearchString(searchStr,findByCode);
        m_catalogView->setCurrentPage(0);
        m_catalogView->update();
        WApplication::instance()->setInternalPath("/shop/1/");

    }

}

void Portal::onEnterPressed()
{
    if(WLineEdit *search = dynamic_cast<WLineEdit*>(sender()))
    {
        if(search->validate())
        {
            log("info")<<" SERACH STR "<<search->text();
            onSearch(search->text());
        }
    }

}

// Создание анкеров в шапке страницы
/*
  currentOrder_ = new WAnchor("/order", tr("order"),contents);
  currentOrder_->setLink(WLink(WLink::InternalPath,"/order"));
  currentOrder_->show();
*/
// Меню Журнала заказов и сообщений
/*
rightTopMenu_ = new WMenu(contents,Wt::Vertical);
rightTopMenu_->setRenderAsList(true);
rightTopMenu_->setStyleClass("view_cart");
//rightTopMenu_->parent()->setStyleClass("u_cart");
mi = rightTopMenu_->addItem
  (tr("order"),wrapView(&Portal::currentOrder), WMenuItem::PreLoading);
mi->itemWidget()->setStyleClass("i");// Стиль анкера меню
mi->itemWidget()->parent()->setStyleClass("u_cart");// Стиль li блока меню
//mi->itemWidget()->parent()->parent()->setStyleClass("view_cart");//Стил ul блока меню
mi = rightTopMenu_->addItem
  (tr("messages"),wrapView(&Portal::messages), WMenuItem::PreLoading);
mi->itemWidget()->setStyleClass("i");// Стиль анкера меню
mi->itemWidget()->parent()->setStyleClass("u_mess");// Стиль li блока меню
*/
/*
logoutMenu_ = new WMenu(m_content,Wt::Vertical);
logoutMenu_->setRenderAsList(true);
logoutMenu_->setStyleClass("u_name");
mi = logoutMenu_->addItem
  (tr("logout"),wrapView(&Portal::logout), WMenuItem::PreLoading);
mi->itemWidget()->setStyleClass("i");// Стиль анкера меню
mi->itemWidget()->parent()->setStyleClass("exit");// Стиль li блока меню
*/
//m_mainTemplate->bindWidget("content",topWidget);
/*
  // Отложенное создание объекта
  //mainMenu_->addItem
  //  (tr("account"), deferCreate(boost::bind(&Portal::blog, this)));

  mainMenu_->addItem
    (tr("news"), deferCreate(boost::bind(&Portal::download, this)),  WMenuItem::PreLoading);

  //mainMenu_->itemSelected().connect(this, &Portal::googleAnalyticsLogger);

*/
/// HTML OUT
//std::ostream& out = std::cout;
//out << "";
//mi->contents()->htmlText(out);
//log("info" ) <<"Menu 2 item service HTML = "<< out;

