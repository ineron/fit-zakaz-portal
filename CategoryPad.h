#ifndef CATEGORYPAD_H
#define CATEGORYPAD_H

#include "../db/DBUtils.h"
#include <Wt/WContainerWidget>

using namespace Wt;
namespace dbo = Wt::Dbo;

class CategoryPad : public Wt::WContainerWidget
{
    public:
        CategoryPad(dbo::Session &session,WContainerWidget *parent = 0);
        virtual ~CategoryPad();
        Signal<WString,WString,int>&itemClicked(){return m_catalogChanged;};

protected:
    void CreateButtons();
    void selectElement();

private: /// variables

    dbo::Session &session_;
    dbo::QueryModel< dbo::ptr<db::GoodsSimple> > *m_model;
    Signal<WString,WString,int>m_catalogChanged;



private: /// procedures
    void readFolders(std::string parent);


};

#endif // CATEGORYPAD_H
