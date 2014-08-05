#ifndef REPORTDEPTSTRUCURE_H
#define REPORTDEPTSTRUCURE_H


#include <Wt/WContainerWidget>
#include <Wt/WStandardItemModel>


using namespace Wt;

class ReportDeptStrucure : public WContainerWidget
{
public:
    ReportDeptStrucure(WStandardItemModel *model,WContainerWidget *parent);
    virtual ~ReportDeptStrucure();
    void setModel(WStandardItemModel *model);
    void updateReport();
protected:
    WStandardItemModel * m_pModel;
    WTable *m_pReport;
private:
    void createReportHeader();
};

#endif // REPORTDEPTSTRUCURE_H
