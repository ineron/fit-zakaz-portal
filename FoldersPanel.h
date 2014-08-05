#ifndef FOLDERSPANEL_H
#define FOLDERSPANEL_H

#include <Wt/WContainerWidget>

using namespace Wt;



class FoldersPanel : public Wt::WContainerWidget
{
public:
        FoldersPanel(WContainerWidget *parent=0);
        virtual ~FoldersPanel();
        void setCurrentPage(int page);
        void update();
        Signal<int>&onPagechanged(){return m_pageChanged;};
protected:
    std::string m_currentFolder;
    std::map<std::string,int> m_ItemPageMap;
private:
    WMenuItem *addMenuItem(const std::string &id,const std::string &parent,const int &page,const int &level,const WString &name,WMenu *catalogFolder);
    WMenuItem *findMenuItem(WMenu *menu,std::string id);
    void changed(WWidget* sender);
    int m_currentPage;
    Signal<int>m_pageChanged;

};

#endif // FOLDERSPANEL_H
