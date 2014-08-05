#ifndef ORDERDETAILS_H
#define ORDERDETAILS_H

#include <Wt/WContainerWidget>
namespace db{
class Catalog;
}


using namespace Wt;

class OrderDetails : public WContainerWidget
{
public:
    OrderDetails(WContainerWidget *parent = 0);
    OrderDetails(std::vector<db::Catalog> orderRecords,WContainerWidget *parent = 0);
    virtual ~OrderDetails();
public:
    void updateReport();
    void setModel(const std::vector<db::Catalog> orderRecords);
protected:
    WTable *m_OrderTables;
    WString *m_OmniComment;
    WContainerWidget *m_TotalAmount;
    WTextArea *m_Comments;
    WContainerWidget *m_CommentsDiv;
    std::vector<db::Catalog> m_OrderRecords;
    void changed();//(WWidget* sender);
    void updateReport(std::vector<db::Catalog> orderRecords);
    void sheduleRender();
private:
    void createHeader();

};

#endif // ORDERDETAILS_H
