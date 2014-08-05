#ifndef REPORTMUTUAL_H
#define REPORTMUTUAL_H

#include <Wt/WContainerWidget>
#include <Wt/WStandardItemModel>


using namespace Wt;


class ReportMutual : public WContainerWidget
{
public:
    ReportMutual(WStandardItemModel *model,WContainerWidget *parent);
    virtual ~ReportMutual();
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

#endif // REPORTMUTUAL_H
