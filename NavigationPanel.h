#ifndef NAVIGATIONPANEL_H
#define NAVIGATIONPANEL_H

#include <Wt/WContainerWidget>

using namespace Wt;

class NavigationPanel : public Wt::WContainerWidget
{
    public:
        NavigationPanel(int maxPages,int currentPage,WContainerWidget *parent = 0);
        virtual ~NavigationPanel();
        Signal<int>&onPagechanged(){return m_pageChanged;};
        int getCurrentPage(){return m_currentPage;};
        void setCurrentPage(int page){m_currentPage = page;update();};
        void setMaxPage(int maxPage){m_maxPages = maxPage;update();};
        void update();
    protected:
    private:
        Signal<int>m_pageChanged;
        int m_maxPages;
        int m_currentPage;
        void changed(WWidget* sender);

    //signals:
    //    void doEvent();
    //private slots:
    //    void onEvent();
};

#endif // NAVIGATIONPANEL_H
