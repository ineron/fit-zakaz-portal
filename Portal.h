// This may look like C code, but it's really -*- C++ -*-
/*
 * Copyright (C) 2008 Emweb bvba, Kessel-Lo, Belgium.
 *
 * See the LICENSE file for terms of use.
 */
#ifndef _PORTAL_H_
#define _PORTAL_H_

#include "Session.h"




#include <Wt/WApplication>
#include <Wt/WContainerWidget>

#include <Wt/WTimer>

#include <boost/array.hpp>
#include <boost/thread.hpp>

#define CATALOG "pager"
#define INLINE_JAVASCRIPT(...) #__VA_ARGS__


class CatalogView;
class NavigationPanel;
class SearchPanel;
class FoldersPanel;


namespace Wt
{
namespace Ext
{
class Menu;
}
class WStackedWidget;
class WTabWidget;
class WTreeNode;
class WTable;
}

class SoapClient;

using namespace Wt;
//using namespace Wt::Ext;

struct Lang
{
    Lang(const std::string& code, const std::string& path,
         const std::string& shortDescription,
         const std::string& longDescription) :
        code_(code),
        path_(path),
        shortDescription_(shortDescription),
        longDescription_(longDescription)
    {
    }

    std::string code_, path_, shortDescription_, longDescription_;
};

/*
 * A utility container widget which defers creation of its single
 * child widget until the container is loaded (which is done on-demand
 * by a WMenu). The constructor takes the create function for the
 * widget as a parameter.
 *
 * We use this to defer widget creation until needed.
 */
/*
template <typename Function>
class DeferredWidget : public WContainerWidget
{
public:
 DeferredWidget(Function f)
   : f_(f) { }

private:
 void load() {
   WContainerWidget::load();
   if (count() == 0)
     addWidget(f_());
 }

 Function f_;
};

template <typename Function>
DeferredWidget<Function> *deferCreate(Function f)
{
 return new DeferredWidget<Function>(f);
}
*/
class Portal : public WApplication
{
public:
    Portal(const WEnvironment& env,
           const std::string& title,
           const std::string& resourceBundle, const std::string& cssPath);

    virtual ~Portal();

    void googleAnalyticsLogger();
    void onClick(WWidget* sender);
    void onPageChanged(int page);
    void onSearch(WString searchStr);
    bool isTerminalMode(){return m_isTerminalMode;};
    Session *getSession(){return &session_;};
    static const int m_maxPage;
    const std::string getLisenseId(){return m_licenseId;};
    const bool getAccesStatus();
    WStackedWidget *getMainContent(){return m_content;};
protected:
    //virtual WWidget *examples() = 0;
    virtual WWidget *createQuoteForm() = 0;
    //virtual WWidget *sourceViewer(const std::string &deployPath) = 0;
    virtual std::string filePrefix() const = 0;

    //static WTimer *m_timer;// = 0;//new WTimer();

    void onAuthEvent();

    void init();

    void addLanguage(const Lang& l)
    {
        languages.push_back(l);
    }
    WWidget *linkSourceBrowser(const std::string& examplePath);

    WTabWidget *examplesMenu_;

    WString tr(const char *key);
    std::string href(const std::string& url, const std::string& description);

    WStackedWidget *m_content;

    //WTemplate *m_mainTemplate;

    void readReleases(WTable *releaseTable);
    void onReportActivate();
private:
    //WContainerWidget *m_domRootHeader;
    //WContainerWidget *m_domRootFooter;
    WContainerWidget *m_rightSide;
    NavigationPanel *m_upPanel;
    NavigationPanel *m_downPanel;
    SearchPanel *m_searchPanel;
    FoldersPanel *m_folderPanel;
    WContainerWidget *m_footer;
    //WTable *releases_;


    WWidget *m_wHeader;
    WWidget *m_pageHeader;
    //WWidget *catalog_;
    WWidget *m_wHomePage;

    CatalogView *m_catalogView;
    //WWidget *rightSideContent_;
    //WStackedWidget *contents_;
    //WAnchor *catalog_hd_;
    //WAnchor *catalog_px_;
    //WAnchor *logout_;
    //WMenu *rightTopMenu_;
    //WMenu *logoutMenu_;


    WWidget *createHome();
    WContainerWidget *createFooter();
    WWidget *createCatalog();
    WWidget *createPreviewButton();
    WWidget *createRightPanel(int mode);
    //WWidget *createNavigationPanel(int currentPage);
    //WWidget * onChangePage(int currentPage);
    void createHeader();
    void logout();
    void loadGoods(WWidget *w);
    void loadContractors();
    void createReportUi(WContainerWidget *parent);


    WWidget *introduction();
    WWidget *blog();
    WWidget *operators();
    WWidget *userCart();
    WWidget *reports();
    WWidget *information();
    WWidget *mutual();
    WWidget *order_journal();
    WWidget *dept_structure();
    WWidget *sales();
    WWidget *orders();
    WWidget *messages();
    WWidget *orderDetails();
    WWidget *services();
    WWidget *whatsnew();
    WWidget *loadexcel();
    WWidget *news();
    WWidget *currentOrder();
    WWidget *privateDate();
    WWidget *ordersJournal();

    WWidget *importOrder();
    WWidget *loadPrice();
    WWidget *terminal();
    WWidget *priceEditor();
    void updateIntoduction(WApplication *app);
    void loadCSS(WApplication *app);
    void setpassword();
    void ordersJournalShow();

    void onEnterPressed();

    //WWidget *createCatalog();
    void requestPassword();
    void userProfile();
    void openDocTable(std::string docId);


    WMenu *mainMenu_;
    WNavigationBar *navigation_;
    //WPopupMenu *subMenuReports_;

    int language_;

    boost::array<WMenuItem* ,8> m_paretItems;
    //WMenuItem *m_paretItems[8];
    //int m_lastRecord;
    //int m_currentPage;
    Session session_;
    void readNews(WTable *newsTable, const std::string& newsfile);

    WWidget *wrapView(WWidget *(Portal::*createFunction)());

    void updateTitle();
    void setLanguage(int language);
    void setLanguageFromPath();
    void setup();
    void logInternalPath(const std::string& path);
    void chatSetUser(const WString& name);
    //void handleInternalPath(const std::string &internalPath);
    WWidget *createCatalogTable();
    WContainerWidget *sideBarContent_;
    std::vector<Lang> languages;

    /// Процедура отправки заказов в 1с
    void sendOrders();
    WPopupMenu *makeShopMenu();
    void createMainMenu();
    std::map<std::string, WPopupMenu*> folderIdNameMap_;
    //Signals

    //friend class CatalogMainPage;
    bool m_isTerminalMode;
    bool m_accessStatus;
    int m_isCatalogMode;
    std::string m_licenseId;
    boost::thread workThread_;
    //JSlot *imgClick;
#ifdef GSOAP
    SoapClient *m_soapClient;
#endif

};

#endif // _PORTAL_H_
