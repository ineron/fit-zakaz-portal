#ifndef CTALOGVIEW_H
#define CTALOGVIEW_H

#include <Wt/WViewWidget>
#include <Wt/WStackedWidget>
#include <Wt/WWidget>

#include "Session.h"

using namespace Wt;

class CatalogView : public WStackedWidget
{
    public:
        enum Mode {Normal,Search};
        CatalogView(int currentPage,WContainerWidget *parent=0);
        virtual ~CatalogView();
        int currentPage() { return m_currentPage; }
        void setCurrentPage(int val) { m_currentPage = val; }
        int lastRecord() { return m_lastRecord; }
        void setLastRecord(int val) { m_lastRecord = val; }
        void setMode(Mode mode){m_mode = mode;};
        void update();
        void setSearchString(const WString& str,bool &findByCode);
    protected:
        WWidget *renderView();

        //WContainerWidget *m_parent;
    private:
        int m_currentPage;
        int m_lastRecord;
        int m_mode;

        //Session session_;
        std::vector<WWidget*> m_editStack;
        WString m_SearchString;

        bool m_findByCode;
    private:
        WWidget *onChangePage();
        WWidget *createCatalog();
        WWidget *createCatalogTable(std::vector<db::Catalog> catalogs,int &tabIndex);
        WWidget *createNavigationPanel();
        void changed(WWidget* sender);
        //WWidget *indexOf(WWidget widget);
};

#endif // CTALOGVIEW_H
