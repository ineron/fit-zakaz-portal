#ifndef REPORTORDERS_H
#define REPORTORDERS_H

#include <Wt/WContainerWidget>
#include <Wt/WStandardItemModel>


using namespace Wt;


class ReportOrders : public WContainerWidget
{
public:
    ReportOrders(WStandardItemModel *model,WContainerWidget *parent);
    virtual ~ReportOrders();
    void setModel(WStandardItemModel *model);
    void updateReport();
protected:
    WStandardItemModel * m_pModel;
    WTable *m_pReport;
private:
    void createReportHeader();
    void onClick(WWidget *sender);
    void openDocTable(std::string docId);
};

#endif // REPORTORDERS_H
