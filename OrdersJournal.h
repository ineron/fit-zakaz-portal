#ifndef ORDERSJOURNAL_H
#define ORDERSJOURNAL_H

#include <Wt/WContainerWidget>

namespace db{class OrdersHeader;}


using namespace Wt;

class OrdersJournal : public WContainerWidget
{
public:
    OrdersJournal(WContainerWidget *parent = 0);
    OrdersJournal(WStandardItemModel *orders,WContainerWidget *parent);

    virtual ~OrdersJournal();

public:
    void updateReport();
    void setModel(WStandardItemModel *ordersModel);

protected:
    WTable *m_OrderTables;
    WStandardItemModel *m_OrdersModel;
    WComboBox *m_groupOperations;
protected:
    void changed(WWidget* sender);
    void onPrint();
    void onRemove();
    void onGroupOeration();
    void updateReport(WStandardItemModel *ordersModel);
    void changeNumebr();
private:
    void createHeader();
    WWidget *changeNumberDialog();
    void inputOrderNumber(WAnchor *anc);

    WDialog *setOrderAddresGroupDoc(std::vector<int> docList);
    void removeGroupDoc(std::vector<int> docList);
    void sendGroupDoc(std::vector<int> docList);
    void copyGroupDoc(std::vector<int> docList);
    void printGroupDoc(std::vector<int> docList);
    void uploadGroupDoc(std::vector<int> docList);

};

#endif // ORDERSJOURNAL_H
