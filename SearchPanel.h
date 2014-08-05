#ifndef SEARCHPANEL_H
#define SEARCHPANEL_H

//#include <Wt/Dbo/Session>
#include <Wt/Dbo/ptr>
#include <Wt/Dbo/Types>
//#include <Wt/Dbo/backend/Postgres>

#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>

#define INLINE_JAVASCRIPT(...) #__VA_ARGS__

using namespace Wt;
namespace dbo = Wt::Dbo;

typedef boost::tuple<std::string> StringPtr;
typedef dbo::collection< StringPtr > StringList;


class SearchPanel : public Wt::WContainerWidget
{
    public:
        SearchPanel(WContainerWidget *parent = 0);
        virtual ~SearchPanel();
        void update();
        Signal<WString>&onSearchChanged(){return m_searchChanged;};
        WString getSearchText() const {return m_searchStr->text();};
        void setSearchText(const WString &str){m_searchStr->setText(str);}
    protected:
        void search();
    private:
        WStringListModel *m_searchModel;

        void readStringModel(WString searchStr);
        WSuggestionPopup *createAliasesMatchingPopup(Wt::WContainerWidget *parent);
        WLineEdit *m_searchStr;
        WString m_searchString;
        Signal<WString>m_searchChanged;

};

#endif // SEARCHPANEL_H
